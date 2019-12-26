#if defined (WIN32)
    #include <winsock2.h>
    typedef int socklen_t;
#elif defined (__unix__)
	#include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(s) close (s)
    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
#else
#error "sockets not supported on this system"
#endif

#include <stdio.h>
#include "http.h"
#include "http_maker.h"
#include "http_parser.h"

http_errors_t http_internal_get_sockaddr(char *addr, void *output, int *addrType) {
    int domain = AF_INET, s;
	*addrType = 0;

    for (size_t i = 0; i < 3; i++) {
        domain = (i) ? AF_INET : AF_INET6;

        if (i == 2) {
			// TODO : resolve code
			return HTTP_ERROR_UNIMPLEMENTED_FEATURE;
        }

        s = inet_pton(domain, addr, output);
        if (s <= 0)
            continue;
        else
            break;
    }

	*addrType = domain;
	return HTTP_ERROR_NOTHING;
}

http_errors_t http_internal_connect(int __fd, const struct sockaddr *__addr, socklen_t __len, uint32_t timeout) {

	struct timeval tv = {0, timeout*1000};
	long arg = 0;
	fd_set fd = {};
	int tmp = 0;
	socklen_t lon = 0;
	int valopt = 0;

	if (tv.tv_usec == 0) {

		if (connect(__fd, __addr, __len) != SOCKET_ERROR) {
			return HTTP_ERROR_NOTHING;
		} else {
			return HTTP_ERROR_CONNECTION;
		}
		
	} else {
			if((arg = fcntl(__fd, F_GETFL, NULL)) < 0) {
			return HTTP_ERROR_UNKOWN;
		}

		arg |= O_NONBLOCK;
		if(fcntl(__fd, F_SETFL, arg) < 0) {
			return HTTP_ERROR_UNKOWN;
		}

		if (connect(__fd, __addr, __len) < 0) {
			if (errno == EINPROGRESS) {
				while (1) {
					FD_ZERO(&fd);
					FD_SET(__fd, &fd);
					tmp = select(__fd+1, NULL, &fd, NULL, &tv);
					if(tmp < 0 && errno != EINTR) {
						return HTTP_ERROR_UNKOWN;
					} else if (tmp > 0) {
						lon = sizeof(int);
						if (getsockopt(__fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
							return HTTP_ERROR_UNKOWN;
						}

						if (valopt) {
							return HTTP_ERROR_UNKOWN;
						} 
						break; 
					} else {
						return HTTP_ERROR_TIMEOUT;
					}
				}
			} else {
				return HTTP_ERROR_UNKOWN;
			}
		} else {
			return HTTP_ERROR_UNKOWN;
		}

		if( (arg = fcntl(__fd, F_GETFL, NULL)) < 0) { 
			return HTTP_ERROR_UNKOWN;
		} 
		arg &= (~O_NONBLOCK); 
		if( fcntl(__fd, F_SETFL, arg) < 0) { 
			return HTTP_ERROR_UNKOWN;
		}

		return HTTP_ERROR_NOTHING;
	}
	
}

http_errors_t http_send_request(http_t request, http_response_t **response) {
	char *request_str = NULL;
	char output[10] = "";
	SOCKET sock;
    unsigned char sin[sizeof(struct sockaddr_in6)] = "";
	size_t sinSize = 0;
	size_t currentSize = 0;
	http_parsing_data parsing_data = {};
	http_errors_t returner = HTTP_ERROR_NOTHING;
	unsigned char addr[sizeof(struct in6_addr)] = "";
	int addrType = 0;

	if (response[0] == NULL) {
		response[0] = calloc(sizeof(http_response_t), 1);

		if (response[0] == NULL) {
			return HTTP_ERROR_ALLOCATION;
		}
		
	}

	parsing_data.response = response[0];

	returner = http_make_request(request, &request_str);

	if (returner != HTTP_ERROR_NOTHING) {
		free(request_str);
		return HTTP_ERROR_UNKOWN;
	}

	if((returner = http_internal_get_sockaddr(request.server, addr, &addrType)) != HTTP_ERROR_NOTHING) {
		free(request_str);
		return returner;
	}

	if (addrType == AF_INET) {

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == SOCKET_ERROR) {
			free(request_str);
			return HTTP_ERROR_UNKOWN;
		}

		((struct sockaddr_in *)sin)->sin_addr.s_addr = inet_addr(request.server);
		((struct sockaddr_in *)sin)->sin_family = AF_INET;
		((struct sockaddr_in *)sin)->sin_port = htons(request.port);
		sinSize = sizeof(struct sockaddr_in);
	} else {

		sock = socket(AF_INET6, SOCK_STREAM, 0);
		if (sock == SOCKET_ERROR) {
			free(request_str);
			return HTTP_ERROR_UNKOWN;
		}

		((struct sockaddr_in6 *)sin)->sin6_addr = *((struct in6_addr *)addr);
		((struct sockaddr_in6 *)sin)->sin6_family = AF_INET6;
		((struct sockaddr_in6 *)sin)->sin6_port = htons(request.port);
		sinSize = sizeof(struct sockaddr_in6);
	}
	

	if((returner = http_internal_connect(sock, (SOCKADDR*)&sin, sizeof(sin), request.timeout)) == HTTP_ERROR_NOTHING) {
		
		send(sock, request_str, http_calc_request_size(request), 0);
		free(request_str);
		request_str = NULL;

		while (currentSize = recv(sock, output, sizeof(output)-1, 0)) {
			
			returner = http_parse_data(&parsing_data, output, currentSize);

			if (returner != HTTP_ERROR_NOTHING)
				break;
		}
	}
	
	closesocket(sock);

	free(request_str);

	http_free_parsing_data(&parsing_data);

	return returner;

}