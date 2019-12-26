#ifndef LIB_HTTP_TOOLS
#define LIB_HTTP_TOOLS

#include <stdlib.h>
#include <stdbool.h>

_Bool  http_internal_isIPv6(const char *addr);
size_t http_internal_write_in_str(size_t offset, const char *str, char *to);
size_t http_internal_calc_num_size(int number);

#endif