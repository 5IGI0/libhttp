#ifndef LIB_HTTP_PARSER
#define LIB_HTTP_PARSER

#include <stdlib.h>

#include "structs.h"

http_errors_t http_parse_data(http_parsing_data *parsingData, char *dataReceived, size_t dataSize);

#endif