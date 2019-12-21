#include <stdlib.h>

size_t http_internal_write_in_str(size_t offset, const char *str, char *to) {
	if (str == NULL)
		return offset;
	
	for (size_t i = 0; str[i] != 0; i++) {
		offset += 1;
		to[offset] = str[i];
	}

	return offset;
}