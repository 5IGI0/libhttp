#ifndef LIB_HTTP_STRUCTS
#define LIB_HTTP_STRUCTS

#include <stdint.h>
#include <stdlib.h>

#define HTTP_BUFFER_SIZES 128

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
typedef enum {
	HTTP_METHOD_GET,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_CONNECT,
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_PATCH,
	HTTP_METHOD_UNSTANDARDISED,
} http_method_t;

#define HTTP_METHOD_UNSTANDARDIZED HTTP_METHOD_UNSTANDARDISED

typedef struct {
	char *name;
	char *value;
} http_header_t;


typedef struct {

	/* server */
	char *server;
	uint16_t port;

	/* connection options */
	uint32_t timeout; // in milliseconds
	uint32_t read_timeout; // in milliseconds

	/* data */
	char *path;

	/* method */
	http_method_t method;
	char *method_name;

	/* headers */
	http_header_t **headers;

	/* body */
	size_t body_size;
	uint8_t *body;

	/* write functions */
	void (*headers_writer)(char *name, char *value, void **to_write);
	void *headers_writer_data;
	void (*body_writer)(uint8_t *data, size_t size, void **write_data);
	void *body_writer_data;

} http_t;

typedef enum {
	HTTP_PREHEADER,
	HTTP_METHODS,
	HTTP_CONTENT
} http_parsing_stages;

typedef struct {

	float http_version;
	uint16_t status_code;

	http_header_t **headers;

	uint8_t *content;
	size_t content_size;

} http_response_t;

typedef struct {
	
	http_parsing_stages stage;
	http_response_t *response;

	char *currentLine;

	/* TODO : rename this fuckin variables */
	size_t currentLineUsed;
	size_t currentLineAlloc;

} http_parsing_data;

typedef enum {
	HTTP_ERROR_NOTHING,
	HTTP_ERROR_UNKOWN,
	HTTP_ERROR_ALLOCATION,
	HTTP_ERROR_CONNECTION,
	HTTP_ERROR_PARSING,
	HTTP_ERROR_INVALID_ARG,
	HTTP_ERROR_TIMEOUT,
	HTTP_ERROR_UNIMPLEMENTED_FEATURE,
	HTTP_ERROR_READ_TIMEOUT
} http_errors_t;


#endif