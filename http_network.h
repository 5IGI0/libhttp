#ifndef LIB_HTTP_NETWORK
#define LIB_HTTP_NETWORK

#include "structs.h"

http_errors_t http_send_request(http_t request, http_response_t **response);

#endif