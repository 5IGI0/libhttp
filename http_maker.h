#ifndef LIB_HTTP_MAKER
#define LIB_HTTP_MAKER

#include "structs.h"
#include <stdlib.h>

size_t http_calc_request_size(http_t request);
http_errors_t http_make_request(http_t request, char **output);

#endif