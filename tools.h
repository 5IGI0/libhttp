#ifndef __HTTP_TOOLS_H__
#define __HTTP_TOOLS_H__

#include "http.h"

char *http_getMethodString(http_method method);
char *http_getMethodStringFromReq(http_request *request);

#endif