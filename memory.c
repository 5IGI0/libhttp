#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "http.h"

//////////////// ERROR ////////////////

http_error *http_intern_getErrorPtr() {
    static http_error *error;
    if(error == NULL) {
        error = calloc(1, sizeof(http_error));
        error[0] = HTTP_ERROR_NO_ERROR;
    }
    return error;
}

void http_intern_setError(http_error error) {
    http_intern_getErrorPtr()[0] = error;
}

http_error http_getLastError() {
    return http_intern_getErrorPtr()[0];
}

//////////////// DYNAMIC ALLOCATION ////////////////

http_request *http_createRequest() {
    http_request *request = calloc(1, sizeof(http_request));

    if(request != NULL) {
        char defaultPath[] = "/";

        request->path = calloc(1, sizeof(defaultPath));
        memcpy(request->path, defaultPath, sizeof(defaultPath));

        request->method = HTTP_METHOD_GET;
        request->port = 80;

        return request;

    }
    
    http_intern_setError(HTTP_ERROR_ALLOCATION);
    
    return NULL;
}

void http_freeRequest(http_request *request) {
    free(request->path);
    free(request->address);
    free(request);
}

bool http_setPath(http_request *request, char *path) {

    if (request == NULL || path == NULL) {

        http_intern_setError(HTTP_ERROR_INVALID_ARGUMENT);

        return false;
    }
    
    char *tmp = calloc(strlen(path)+1, 1);

    if (tmp == NULL) {
        http_intern_setError(HTTP_ERROR_ALLOCATION);
        return false;
    }

    memcpy(tmp, path, strlen(path)+1);
    free(request->path);
    request->path = tmp;

    return true;
}

bool http_setAddress(http_request *request, char *addr) {

    if (request == NULL || addr == NULL) {

        http_intern_setError(HTTP_ERROR_INVALID_ARGUMENT);

        return false;
    }
    
    char *tmp = calloc(strlen(addr)+1, 1);

    if (tmp == NULL) {
        http_intern_setError(HTTP_ERROR_ALLOCATION);
        return false;
    }

    memcpy(tmp, addr, strlen(addr)+1);
    free(request->address);
    request->address = tmp;

    return true;
}

bool http_setCustomMethod(http_request *request, char *method) {

    if (request == NULL || method == NULL) {

        http_intern_setError(HTTP_ERROR_INVALID_ARGUMENT);

        return false;
    }
    
    char *tmp = calloc(strlen(method)+1, 1);

    if (tmp == NULL) {
        http_intern_setError(HTTP_ERROR_ALLOCATION);
        return false;
    }

    memcpy(tmp, method, strlen(method)+1);
    free(request->custom_method);
    request->custom_method = tmp;
    request->method = HTTP_METHOD_CUSTOM;

    return true;
}