#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "http.h"
#include "structs.h"

size_t http_internal_len_line(char *line, size_t text_size /* security */) {
	size_t i = 0;

	for (; line[i] != '\n' && line[i] != '\0' && i < text_size+1; i++) {}

	return i;
}

http_errors_t http_parse_data(http_parsing_data *parsingData, char *dataReceived, size_t dataSize) {
	size_t tmp = 0;
	void *tmp_ptr = NULL;

	for (size_t dataOffset = 0; dataOffset < dataSize; dataOffset++) {
		if (parsingData->currentLine != NULL) {
			if (parsingData->currentLine[parsingData->currentLineUsed-1] == '\n') {
				memset(parsingData->currentLine, 0, parsingData->currentLineUsed);
				parsingData->currentLineUsed = 0;
			}
		}

		if (parsingData->stage < HTTP_CONTENT) {

			tmp = http_internal_len_line(dataReceived+dataOffset, dataSize-dataOffset-1);
			
			if (parsingData->currentLine == NULL) {
				parsingData->currentLine = calloc(tmp+2, 1);

				if (parsingData->currentLine == NULL) {
					return HTTP_ERROR_ALLOCATION;
				}

				parsingData->currentLineAlloc = tmp+1;
			} else if (tmp+1+parsingData->currentLineUsed > parsingData->currentLineAlloc) {
				tmp_ptr = realloc(parsingData->currentLine, tmp+2+parsingData->currentLineUsed);

				if (tmp_ptr == NULL) {
					return HTTP_ERROR_ALLOCATION;
				} else
					parsingData->currentLine = tmp_ptr;

				memset(parsingData->currentLine+parsingData->currentLineUsed+1, 0, tmp+1);

				parsingData->currentLineAlloc = tmp+1+parsingData->currentLineUsed;
			}


			memcpy(parsingData->currentLine+parsingData->currentLineUsed, dataReceived+dataOffset, tmp+1);
			parsingData->currentLineUsed = strlen(parsingData->currentLine);

			dataOffset += tmp;

			if (parsingData->currentLine[parsingData->currentLineUsed-1] != '\n')
				break;
			
		} else {

			if (parsingData->response->content_size == 0) {
				
				parsingData->response->content = calloc(dataSize-dataOffset, 1);

				if (parsingData->response->content == NULL)
					return HTTP_ERROR_ALLOCATION;

				memcpy(parsingData->response->content, dataReceived+dataOffset, dataSize-dataOffset);

				parsingData->response->content_size = dataSize-dataOffset;

				break;

			} else {
				
				parsingData->response->content = realloc(parsingData->response->content, parsingData->response->content_size+dataSize);

				if (parsingData->response->content == NULL)
					return HTTP_ERROR_ALLOCATION;

				memcpy(parsingData->response->content+parsingData->response->content_size, dataReceived, dataSize);

				parsingData->response->content_size += dataSize;

				break;

			}
			
		}
	
		if (parsingData->stage == HTTP_PREHEADER) {
			if(sscanf(parsingData->currentLine, "HTTP/%f %hu", &parsingData->response->http_version, &parsingData->response->status_code) != 2)
				return HTTP_ERROR_PARSING;
			
			parsingData->stage = HTTP_METHODS;
		} else if (parsingData->stage == HTTP_METHODS) {
			char *name = NULL;
			char *value = NULL;

			if (strlen(parsingData->currentLine) == 2) {
				parsingData->stage = HTTP_CONTENT;
				continue;
			}

			tmp = 0;

			for (; parsingData->currentLine[tmp] != ':' && parsingData->currentLine[tmp+1] != ' '; tmp++) { // TODO: may cause segmentation fault. (because of [tmp+1] (exceeding the string in case of invalid format))
				if (parsingData->currentLine[tmp] == 0) {
					return HTTP_ERROR_PARSING;
				}
			}

			name = calloc(tmp+1, 1);

			if (name == NULL)
				return HTTP_ERROR_ALLOCATION;

			memcpy(name, parsingData->currentLine, tmp);

			value = calloc(parsingData->currentLineUsed-tmp-3, 1);

			if (value == NULL) {
				free(name);
				return HTTP_ERROR_ALLOCATION;
			}

			memcpy(value, parsingData->currentLine+tmp+2, parsingData->currentLineUsed-tmp-4);

			http_add_header(name, value, &parsingData->response->headers);

			free(name);
			free(value);
		}
	}

	return HTTP_ERROR_NOTHING;
}