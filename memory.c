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
    
    if(request->headers != NULL) {
        for (size_t i = 0; request->headers[i] != NULL; i++) {
            free(request->headers[i]->value);
            free(request->headers[i]->key);
            free(request->headers[i]);
        }

        free(request->headers);

    }
    
    free(request->custom_method);
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

bool http_setHeader(http_request *request, char *key, char *value) {

    if(request->headers == NULL) {
        
        request->header_size = 0;
        request->headers = calloc(2, sizeof(http_header *));

        if(request->headers == NULL)
            return false;

    } else {
        
        http_header **headers = request->headers;
        request->headers = realloc(request->headers, (request->header_size+2)*sizeof(http_header *));

        if(request->headers == NULL) {
            request->headers = headers;
            return false;
        }

        request->headers[request->header_size+1] = NULL;

    }

    request->header_size++;

    request->headers[request->header_size-1] = calloc(1, sizeof(http_header));
    
    if(request->headers[request->header_size-1] == NULL) return false;

    request->headers[request->header_size-1]->key = calloc(strlen(key)+1, 1);

    if (request->headers[request->header_size-1]->key == NULL) {
        free(request->headers[request->header_size-1]);
        request->headers[request->header_size-1] = NULL;
        request->header_size--;
        return false;
    }
    
    request->headers[request->header_size-1]->value = calloc(strlen(value)+1, 1);

    if (request->headers[request->header_size-1]->value == NULL) {
        free(request->headers[request->header_size-1]->key);
        free(request->headers[request->header_size-1]);
        request->headers[request->header_size-1] = NULL;
        request->header_size--;
        return false;
    }

    memcpy(request->headers[request->header_size-1]->key, key, strlen(key));
    memcpy(request->headers[request->header_size-1]->value, value, strlen(value));

    return true;
}

char *http_intern_getHeaderValue(char *key, http_header **headers) {

    for (size_t i = 0; headers[i] != NULL; i++)
        if(strcmp(key, headers[i]->key) == 0) return headers[i]->value;
    
    return NULL;

}