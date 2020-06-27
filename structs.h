#ifndef __HTTP_STRUCTS_H__
#define __HTTP_STRUCTS_H__

#include <stdint.h>
#include <netdb.h>

typedef enum __http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_CUSTOM
} http_method;

typedef struct __http_header {
    char *key;
    char *value;
} http_header;

typedef struct __http_request {

    // target
    char *address;
    uint16_t port;

    // request
    char *path;
    http_method method;
    char *custom_method;
    http_header **headers;
    size_t header_size;
    
    // reading
    size_t (*data_writer) (uint8_t *data, size_t n, void *obj);
    void *data_obj;
    size_t (*header_writer) (char *key, char *value, void *obj);
    void *header_obj;

} http_request;

typedef enum __http_error {
    HTTP_ERROR_NO_ERROR,
    HTTP_ERROR_ALLOCATION,
    HTTP_ERROR_INVALID_ARGUMENT,
    HTTP_ERROR_RESOLVE_ADDR,
    HTTP_ERROR_CONNECTION_ERROR,
    HTTP_ERROR_SYSTEM,
    HTTP_ERROR_CONNECTION_TIMEOUT,
    HTTP_ERROR_COMMUNICATION_ERROR,
    HTTP_ERROR_WRITING_ERROR
} http_error;

#endif