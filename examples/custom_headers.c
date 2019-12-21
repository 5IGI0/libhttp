#include "../http.h"

#include <stdio.h>

int main(int argc, char const *argv[]) {
	char *request_str = NULL;
	http_t request = {
		.server = "127.0.0.1",
		.port = 80,
		.path = "/",
		.method = HTTP_METHOD_GET
	};
	http_response_t *response = NULL;
	http_errors_t error = HTTP_ERROR_NOTHING;


	/* SETUP CUSTOM HEADERS */
	http_add_header("User-Agent", "MyBot :3", &request.headers);
	http_add_header("Accept-Language", "fr-FR", &request.headers); // Les Français parlent aux Français :flag_fr:

	error = http_send_request(request, &response);

	if (error != HTTP_ERROR_NOTHING) {
		fprintf(stderr, "error ! ;'(\n");
		exit(EXIT_FAILURE);
	}

	printf("Headers : \n\n");

	for (size_t i = 0; response->headers[i] != NULL; i++) {
		printf("%s: %s\n", response->headers[i]->name, response->headers[i]->value);
	}

	printf("data (%ld) : \n\n", response->content_size);

	for (size_t i = 0; i < response->content_size; i++)
		printf("%c", response->content[i]);

	printf("\n");

	return 0;
}