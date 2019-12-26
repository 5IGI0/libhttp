#include "../http.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	http_t request = {
		.server = "::1",
		.port = 80,
		.path = "/",
		.method = HTTP_METHOD_GET
	};

	http_response_t *response = NULL;
	http_errors_t error = HTTP_ERROR_NOTHING;

	error = http_send_request(request, &response);

	if (error != HTTP_ERROR_NOTHING) {
		fprintf(stderr, "error ! ;'(\n");
		exit(EXIT_FAILURE);
	}

	printf("data (%ld) : \n\n", response->content_size);

	for (size_t i = 0; i < response->content_size; i++)
		printf("%c", response->content[i]);

	printf("\n");

	return 0;
}