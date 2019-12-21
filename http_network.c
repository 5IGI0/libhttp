#if defined (WIN32)
    #include <winsock2.h>
    typedef int socklen_t;
#elif defined (__unix__)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
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

http_errors_t http_send_request(http_t request, http_response_t **response) {
	char *request_str = NULL;
	char output[10] = "";
	SOCKET sock;
    SOCKADDR_IN sin;
	size_t currentSize = 0;
	http_parsing_data parsing_data = {};
	http_errors_t returner = HTTP_ERROR_NOTHING;

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
		request_str = NULL;
	}
	
 
	sock = socket(AF_INET, SOCK_STREAM, 0);

	sin.sin_addr.s_addr = inet_addr(request.server);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(request.port);

	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR) {
		
		send(sock, request_str, http_calc_request_size(request), 0);
		free(request_str);

		while (currentSize = recv(sock, output, sizeof(output)-1, 0)) {
			
			returner = http_parse_data(&parsing_data, output, currentSize);

			if (returner != HTTP_ERROR_NOTHING)
				break;
		}
	} else {
		returner = HTTP_ERROR_CONNECTION;
	}
	
	closesocket(sock);

	http_free_parsing_data(&parsing_data);

	return returner;

}