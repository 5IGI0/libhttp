#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

/* HEADERS */

http_errors_t http_add_header(char *name, char *value, http_header_t ***headers) {
	http_header_t *header = NULL;
	size_t headerAmount = 0;
	void *tmp_ptr = NULL;

	if (name == NULL)
		return HTTP_ERROR_INVALID_ARG;
	else if (value == NULL)
		return HTTP_ERROR_INVALID_ARG;
	
	header = calloc(sizeof(http_header_t), 1);

	if (header == NULL) {
		return HTTP_ERROR_ALLOCATION;
	}

	header->name = calloc(strlen(name)+1, 1);

	if (header->name == NULL) {
		free(header);
		return HTTP_ERROR_ALLOCATION;
	}
	
	header->value = calloc(strlen(value)+1, 1);

	if (header->value == NULL) {
		free(header->name);
		free(header);
		return HTTP_ERROR_ALLOCATION;
	}

	strcpy(header->name, name);
	strcpy(header->value, value);

	if (headers[0] == NULL) {
		headers[0] = calloc(sizeof(http_header_t *), 2);

		if (headers[0] == NULL) {
			free(header->name);
			free(header->value);
			free(header);
			return HTTP_ERROR_ALLOCATION;
		}

		headerAmount = 0;
	} else {
		for (size_t i = 0; headers[0][i] != NULL; i++)
			headerAmount ++;
		
		tmp_ptr = realloc(headers[0], (headerAmount+2)*sizeof(http_header_t *));

		if (tmp_ptr == NULL) {
			free(header->name);
			free(header->value);
			free(header);
			return HTTP_ERROR_ALLOCATION;
		}

		headers[0] = tmp_ptr;
	}
	
	headers[0][headerAmount] = header;
	headers[0][headerAmount+1] = NULL;

	return HTTP_ERROR_NOTHING;

}

void http_remove_header(char *headerName, http_header_t ***headers) {
	size_t offset = 0;
	size_t y = 0;

	for (; headers[0][offset] != NULL; offset++) {
		if (!strcmp(headers[0][offset]->name, headerName)) {

			http_free_header(headers[0][offset]);
			free(headers[0][offset]);

			for (y = offset+1; headers[0][y] != NULL; y++) {
				headers[0][y-1] = headers[0][y];
			}

			headers[0] = realloc(headers[0], sizeof(http_header_t)*y);

			break;
		}
	}
}