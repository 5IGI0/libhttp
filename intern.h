#ifndef __HTTP_INTERN_H__
#define __HTTP_INTERN_H__

#include "structs.h"

void http_intern_setError(http_error error);
char *http_intern_getHeaderValue(char *key, http_header **headers); 

#endif