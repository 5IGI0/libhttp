#include "../http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* WRITER DATA */

typedef struct body_writer_data {
	FILE *file;
	char *filename;
} body_writer_data_t;



/* WRITER FUNCTIONS */

void header_writer(char *key, char *value, void **data) {
	printf("new header : \"%s\" -> \"%s\"\n", key, value);
}

void body_writer(uint8_t *data, size_t size, void **write_data_tmp) {
	body_writer_data_t **write_data = (body_writer_data_t **)write_data_tmp;

	if (write_data[0]->file == NULL) {
		write_data[0]->file = fopen(write_data[0]->filename, "wb");
		
		if (write_data[0]->file == NULL) {
			perror("");
			exit(EXIT_FAILURE);
		}
	}

	fwrite(data, 1, size, write_data[0]->file);	
}

int main(int argc, char const *argv[]) {
	http_t request = {
		.server = "127.0.0.1",
		.port = 80,
		.path = "/",
		.method = HTTP_METHOD_GET,
		.headers_writer = &header_writer,
		.body_writer = &body_writer
	};

	body_writer_data_t data = {
		.filename = "untitled.txt"
	};

	request.body_writer_data = &data;

	http_response_t *response = NULL;
	http_errors_t error = HTTP_ERROR_NOTHING;

	error = http_send_request(request, &response);

	if (data.file != NULL) {
		fclose(data.file);
	}
	

	if (error != HTTP_ERROR_NOTHING) {
		fprintf(stderr, "error ! ;'(\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}