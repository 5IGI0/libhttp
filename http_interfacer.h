#ifndef LIB_HTTP_INTERFACER
#define LIB_HTTP_INTERFACER

#include "structs.h"

http_errors_t http_add_header(char *name, char *value, http_header_t ***headers);
void http_remove_header(char *headerName, http_header_t ***headers);

#endif