#include <stdio.h>
#include "http.h"

char *http_getMethodString(http_method method) {
    
    switch (method) {
        case HTTP_METHOD_GET:
            return "GET";

        case HTTP_METHOD_POST:
            return "POST";
        
        default:
            return NULL;
    }
}

char *http_getMethodStringFromReq(http_request *request) {

    if (request->method == HTTP_METHOD_CUSTOM)
        return request->custom_method;
    else
        return http_getMethodString(request->method);
}