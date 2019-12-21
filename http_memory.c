#include <stdlib.h>

#include "http.h"

void http_free_parsing_data(http_parsing_data *parsingData) {
	free(parsingData->currentLine);
}

void http_free_header(http_header_t *header) {
	free(header->name);
	free(header->value);
}

void http_free_headers(http_header_t ***headers) {
	for (size_t i = 0; headers[0][i] != NULL; i++) {
		http_free_header(headers[0][i]);
		free(headers[0][i]);
	}

	free(headers[0]);
}