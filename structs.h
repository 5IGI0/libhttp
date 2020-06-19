#ifndef __HTTP_STRUCTS_H__
#define __HTTP_STRUCTS_H__

#include <stdint.h>

typedef enum __http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_CUSTOM
} http_method;

typedef struct __http_request {

    // target
    char *address;
    uint16_t port;

    // request
    char *path;
    http_method method;
    char *custom_method;

} http_request;

typedef enum __http_error {
    HTTP_ERROR_NO_ERROR,
    HTTP_ERROR_ALLOCATION,
    HTTP_ERROR_INVALID_ARGUMENT
} http_error;

#endif