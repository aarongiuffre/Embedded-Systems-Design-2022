/*
 * json.h
 *
 *  Created on: Mar 2, 2022
 *      Author: acemc
 */

#ifndef JSON_H_
#define JSON_H_

#define JSMN_HEADER
#include "jsmn.h"

#define MAX_TOKENS 12
#define JSON_BUFFER_LEN 64
#define INT_MIN -2147483648

int parse_int_token(jsmntok_t t, char *str);
float parse_float_token(jsmntok_t t, char *str);

#endif /* JSON_H_ */
