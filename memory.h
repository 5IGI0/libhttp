#ifndef __HTTP_MEMORY_H__
#define __HTTP_MEMORY_H__

#include <stdbool.h>

#include "structs.h"

http_error http_getLastError();

http_request *http_createRequest();
void http_freeRequest(http_request *request);

bool http_setAddress(http_request *request, char *addr);
bool http_setPath(http_request *request, char *path);
bool http_setCustomMethod(http_request *request, char *method);
bool http_setHeader(http_request *request, char *key, char *value);

#endif