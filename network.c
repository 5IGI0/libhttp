#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>


#include "http.h"
#include "intern.h"

bool http_intern_network_connect(int socket, struct sockaddr *addr, size_t addrlen) {
    int res = 0;  
    long arg  = 0; 
    fd_set myset = {}; 
    struct timeval tv = {.tv_sec=10}; 
    int valopt = 0; 
    socklen_t lon = 0;

    if( (arg = fcntl(socket, F_GETFL, NULL)) < 0) { 
        http_intern_setError(HTTP_ERROR_SYSTEM);
        return false;
    }

    arg |= O_NONBLOCK; 
    if( fcntl(socket, F_SETFL, arg) < 0) { 
        http_intern_setError(HTTP_ERROR_SYSTEM);
        return false;
    }

    res = connect(socket, addr, addrlen); 

    if (res < 0) {
        if(errno == EINPROGRESS) {
            while(1) {

                FD_ZERO(&myset); 
                FD_SET(socket, &myset);

                res = select(socket+1, NULL, &myset, NULL, &tv); 

                if (res < 0 && errno != EINTR) {
                    http_intern_setError(HTTP_ERROR_CONNECTION_ERROR);
                    return false;
                } else if (res > 0) {
                    lon = sizeof(int);

                    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                        http_intern_setError(HTTP_ERROR_SYSTEM);
                        return false;
                    }

                    if (valopt) {
                        errno = valopt;
                        if(errno == ECONNREFUSED) {
                            http_intern_setError(HTTP_ERROR_CONNECTION_ERROR);
                        } else {
                            http_intern_setError(HTTP_ERROR_SYSTEM);
                        }
                        return false;
                    }

                    break;
                } else {
                    http_intern_setError(HTTP_ERROR_CONNECTION_TIMEOUT);
                    return false;
                }
            }
        } else {
            http_intern_setError(HTTP_ERROR_CONNECTION_ERROR);
            return false;
        }
    }

 
    if( (arg = fcntl(socket, F_GETFL, NULL)) < 0) { 
        http_intern_setError(HTTP_ERROR_SYSTEM);
        return false;
    }
    
    arg &= (~O_NONBLOCK); 
    if( fcntl(socket, F_SETFL, arg) < 0) { 
       http_intern_setError(HTTP_ERROR_SYSTEM);
       return false;
    }

    return true;
}

int http_intern_connect(http_request *request) {
    struct addrinfo hints = {};
    struct addrinfo *result, *rp;
    char port[6] = "\0";
    int r = 0;
    int tmpsock = 0;
    sprintf(port, "%d", request->port);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /*       TCP/IP       */

    r = getaddrinfo(request->address, port, &hints, &result);

    if(r != 0) {
        http_intern_setError(HTTP_ERROR_RESOLVE_ADDR);
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        tmpsock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (tmpsock == -1) continue;

        if (http_intern_network_connect(tmpsock, rp->ai_addr, rp->ai_addrlen)) return tmpsock;
    }

    return -1;
}

bool http_intern_send(int __fd, const void *__buf, size_t __n, int __flags) {
    if(send(__fd, __buf, __n, __flags) != __n) {
        http_intern_setError(HTTP_ERROR_COMMUNICATION_ERROR);
        return false;
    }

    return true;
}

bool http_execute(http_request *request) {
    int socket = http_intern_connect(request);

    if(socket == -1) return false;

    if(!http_intern_send(
            socket,
            http_getMethodStringFromReq(request),
            strlen(http_getMethodStringFromReq(request)),
            MSG_NOSIGNAL
        )
    ) return false;

    if(!http_intern_send(socket, " ", 1, MSG_NOSIGNAL)) return false;
    if(request->path[0] != '/') {
        if(!http_intern_send(socket, "/", 1, MSG_NOSIGNAL)) return false;
    }
    if(!http_intern_send(socket, request->path, strlen(request->path), MSG_NOSIGNAL)) return false;
    if(!http_intern_send(socket, " HTTP/1.1\r\n", 11, MSG_NOSIGNAL)) return false;
    if(!http_intern_send(socket, "Connection: close\r\n", 19, MSG_NOSIGNAL)) return false;
    if(!http_intern_send(socket, "Host: ", 6, MSG_NOSIGNAL)) return false;
    if(!http_intern_send(socket, request->address, strlen(request->address), MSG_NOSIGNAL)) return false;
    if(!http_intern_send(socket, "\r\n", 2, MSG_NOSIGNAL)) return false;
    
    if(request->headers != NULL) {
        for (size_t i = 0; request->headers[i] != NULL; i++) {
            if(!http_intern_send(socket, request->headers[i]->key, strlen(request->headers[i]->key), MSG_NOSIGNAL)) return false;
            if(!http_intern_send(socket, " ", 1, MSG_NOSIGNAL)) return false;
            if(!http_intern_send(socket, request->headers[i]->value, strlen(request->headers[i]->value), MSG_NOSIGNAL)) return false;
            if(!http_intern_send(socket, "\r\n", 2, MSG_NOSIGNAL)) return false;
        }
    }

    if(!http_intern_send(socket, "\r\n", 2, MSG_NOSIGNAL)) return false;

    uint8_t buffer[53] = {};
    ssize_t rcved = 0;
    char *dataStart = NULL;
    bool is_data = false;

    while (rcved = recv(socket, buffer, 50, MSG_NOSIGNAL)) {
        if(rcved < 1) break;
        if(is_data == false) {
            if((dataStart = strstr(buffer, "\r\n\r\n")) != NULL) {
                is_data=true;
                rcved -= ((dataStart+4)-((char *)buffer));
                memcpy(buffer, dataStart+4, rcved);
            }
        }

        if(is_data) {
            if(request->data_writer == NULL) {
                for (size_t i = 0; i < rcved; i++) {
                    putc(buffer[i], stdout);
                }
            } else {
                if(request->data_writer(buffer, rcved, request->data_obj) != rcved) {
                    http_intern_setError(HTTP_ERROR_WRITING_ERROR);
                    close(socket);
                    return false;
                }
            }
        }
    }

    return true;
}