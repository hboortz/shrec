#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct {
    int nvk;
    int pos;
} Event;

char *eventToString(Event event);
Event stringToEvent(char *string);


#endif