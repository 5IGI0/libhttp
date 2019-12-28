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

void http_free_response(http_response_t *response) {
	/* WARNING : this function don't free the response but the response attributes */

	free(response->content);
	response->content = NULL;
	response->content_size = 0;

	http_free_headers(&response->headers);
	response->headers = NULL;

	response->http_version = 0.0;

	response->status_code = 0;
}

void http_free_request(http_t *request) {
	/* WARNING : this function don't free the request but the request attributes */

	// timeouts
	request->timeout = 0;
	request->read_timeout = 0;

	// server
	free(request->server);
	request->server = NULL;
	request->port = 0;
	free(request->path);
	request->path = NULL;

	// body
	free(request->body);
	request->body = NULL;
	request->body_size = 0;

	// headers
	http_free_headers(&request->headers);
	request->headers = NULL;

	// method
	free(request->method_name);
	request->method_name = NULL;
}