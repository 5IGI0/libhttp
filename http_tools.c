#include <stdlib.h>
#include <math.h>

size_t http_internal_write_in_str(size_t offset, const char *str, char *to) {
	if (str == NULL)
		return offset;
	
	for (size_t i = 0; str[i] != 0; i++) {
		offset += 1;
		to[offset] = str[i];
	}

	return offset;
}

size_t http_internal_calc_num_size(int number) {
	size_t i = 0;
	unsigned int size = 1;
	while(number/size > 9) {
		size = pow(10,i);
		i += 1;
	}

	if (i == 0)
		i = 1;
	
	return i;
}