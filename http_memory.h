#ifndef LIB_HTTP_MEMORY
#define LIB_HTTP_MEMORY

#include "http.h"

void http_free_parsing_data(http_parsing_data *parsingData);
void http_free_header(http_header_t *header);
void http_free_headers(http_header_t ***headers);

#endif