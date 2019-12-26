#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "http.h"
#include "http_tools.h"

#define COUNT_OF(x) (sizeof(x)/sizeof(x[0]))

size_t GET_method_size(http_t request) {
	static size_t method_sizes[] = {
		3, // GET
		4, // HEAD,
		4, // POST
		3, // PUT
		6, // DELETE
		7, // CONNECT
		7, // OPTIONS
		5, // TRACE
		5 // PATCH
	};

	if (request.method <= COUNT_OF(method_sizes)) { 
		return method_sizes[request.method];
	} else {
		if (request.method_name == NULL)
			return 0;
		return strlen(request.method_name);
	}
}

const char *GET_method_string(http_t request) {
	static char *methods[] = {
		"GET", // GET
		"HEAD", // HEAD,
		"POST", // POST
		"PUT", // PUT
		"DELETE", // DELETE
		"CONNECT", // CONNECT
		"OPTIONS", // OPTIONS
		"TRACE", // TRACE
		"PATCH" // PATCH
	};

	if (request.method <= COUNT_OF(methods)) { 
		return methods[request.method];
	} else {
		if (request.method_name == NULL)
			return NULL;
		return request.method_name;
	}
}

_Bool header_exist(char *headerName, http_t request) {
	if (headerName != NULL && request.headers != NULL) {
		for (size_t i = 0; request.headers[i] != NULL; i++) {
			if (!strcmp(request.headers[i]->name, headerName))
				return true;
		}
	}

	return false;
}

size_t http_calc_request_size(http_t request) {
	size_t size = 0;

	size += GET_method_size(request);

	if (size == 0)
		return size;
	

	/* GET[THIS SPACE]/ HTTP/1.1 */
	size += 1;

	if (request.path == NULL)
		size += 1; // /
	else {
		size += strlen(request.path);

		if (request.path[0] != '/')
			size += 1; // /
	}

	/* GET /[THIS SPACE]HTTP/1.1 */
	size += 1;

	size += 10; // HTTP/1.1\r\n

	if (!header_exist("host", request)) {
		
		size += 6; // "host: "
		if (request.server == NULL)
			return 0;

		if (http_internal_isIPv6(request.server))
			size += 2; // [<host>]
		
		size += strlen(request.server);

		size += 2; // \r\n
	}

	if (!header_exist("Accept-Encoding", request)) {
		size += 26; //Accept-Encoding: deflate\r\n
	}
	
	size += 19; //Connection: close\r\n

	if (request.body_size != 0) {
		size += 16;// "Content-Length: "
		size += http_internal_calc_num_size(request.body_size);
		size += 1; // \r\n
	}
	

	/* headers */
	if (request.headers != NULL) {
		for (size_t i = 0; request.headers[i] != NULL; i++) {
			size += strlen(request.headers[i]->name);
			size += 3; // ": "
			size += strlen(request.headers[i]->value);
			size += 2; // \r\n
		}
	}

	size += 2; // \r\n

	if (request.body_size != 0) {
		size += 2; // \r\n
		size += request.body_size;
	}
	

	return size;
}

http_errors_t http_make_request(http_t request, char **output) {
	size_t offset = http_calc_request_size(request); // temporally used for calcul the request size
	size_t tmp;

	if (offset == 0)
		return HTTP_ERROR_INVALID_ARG;
	
	output[0] = calloc(offset, 1);

	if (output[0] == NULL) {
		return HTTP_ERROR_ALLOCATION;
	}
	
	offset = -1;
	offset = http_internal_write_in_str(offset, GET_method_string(request), output[0]);

	if (offset == 0)
		return HTTP_ERROR_INVALID_ARG;

	offset = http_internal_write_in_str(offset, " ", output[0]);

	if (request.path == NULL)
		offset = http_internal_write_in_str(offset, "/", output[0]);
	else {

		if (request.path[0] != '/')
			offset = http_internal_write_in_str(offset, "/", output[0]);

		offset = http_internal_write_in_str(offset, request.path, output[0]);
	}

	offset = http_internal_write_in_str(offset, " HTTP/1.1\r\n", output[0]);

	if (!header_exist("host", request)) {
		_Bool IPv6 = http_internal_isIPv6(request.server);
		offset = http_internal_write_in_str(offset, "host: ", output[0]);
		if (IPv6)
			offset = http_internal_write_in_str(offset, "[", output[0]);
		offset = http_internal_write_in_str(offset, request.server, output[0]);
		if (IPv6)
			offset = http_internal_write_in_str(offset, "]", output[0]);
		offset = http_internal_write_in_str(offset, "\r\n", output[0]);
	}

	if (!header_exist("Accept-Encoding", request)) {
		offset = http_internal_write_in_str(offset, "Accept-Encoding: deflate\r\n", output[0]);
	}
	
	offset = http_internal_write_in_str(offset, "Connection: close\r\n", output[0]);

	if (request.body_size != 0) {
		offset = http_internal_write_in_str(offset, "Content-Length: ", output[0]);
		offset += 1;
		sprintf(output[0]+offset, "%ld", request.body_size);
		offset += http_internal_calc_num_size(request.body_size)-1;
		offset = http_internal_write_in_str(offset, "\r\n", output[0]);
	}

	/* headers */
	if (request.headers != NULL) {
		for (size_t i = 0; request.headers[i] != NULL; i++) {
			offset = http_internal_write_in_str(offset, request.headers[i]->name, output[0]);
			offset = http_internal_write_in_str(offset, ": ", output[0]);
			offset = http_internal_write_in_str(offset, request.headers[i]->value, output[0]);
			offset = http_internal_write_in_str(offset, "\r\n", output[0]);
		}
	}

	offset = http_internal_write_in_str(offset, "\r\n", output[0]);

	if (request.body_size != 0) {
		offset = http_internal_write_in_str(offset, "\r\n", output[0]);
		memcpy(output[0]+offset, request.body, request.body_size);
	}

	return HTTP_ERROR_NOTHING;
}