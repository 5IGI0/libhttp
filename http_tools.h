#ifndef LIB_HTTP_TOOLS
#define LIB_HTTP_TOOLS

#include <stdlib.h>

size_t http_internal_write_in_str(size_t offset, const char *str, char *to);

#endif