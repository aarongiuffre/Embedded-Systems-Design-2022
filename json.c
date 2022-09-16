/*
 * json.c
 *
 *  Created on: Mar 2, 2022
 *      Author: acemc
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "json.h"

int parse_int_token(jsmntok_t t, char *str) {
    static char buff[JSON_BUFFER_LEN];
    uint8_t length = t.end - t.start;

    if (length >= JSON_BUFFER_LEN) {
        return INT_MIN;
    }

    int i;
    for (i = 0; i < length; i ++) {
        buff[i] = str[i + t.start];
    }
    buff[length] = '\0';
    return atoi(buff);
}

float parse_float_token(jsmntok_t t, char *str) {
    static char buff[JSON_BUFFER_LEN];
    uint8_t length = t.end - t.start;

    if (length >= JSON_BUFFER_LEN) {
        return INT_MIN;
    }

    int i;
    for (i = 0; i < length; i ++) {
        buff[i] = str[i + t.start];
    }
    buff[length] = '\0';
    return atof(buff);
}
