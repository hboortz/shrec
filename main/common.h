#ifndef COMMON_H
#define COMMON_H

#define MAX_MSG_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <iostream>
#include <queue>
#include <deque>
using namespace std;

//TODOS
//  Fix mallocated sizes with MAX_MSG_SIZE
//   Make these standard across client and server
//  Implement ADD_STRING and maybe CURSOR_MOVE
//  Copy/paste
//  Figure out a good method of synchronizing cursor positions 
//  Make filename match the date & time of the first connection


// Used to record key events
typedef struct {
    int nvk; //key identifier
    int pos; //location within text field
} Event;

// Used to define the type of message sent
typedef enum {
    KEY_EVENT,
    ADD_STRING,
    REMOVE_STRING,
    INITIAL_SEND,
    CURSOR_MOVE
} Action;

char *eventToString(Event event);
Event stringToEvent(char *string);
void addAction(Action *action, char* string);
void popAction(Action *action, char* string);
void popMetadata(char **inputString, Action *action, int *size, char **msg);
void addMetadata(const Action action, char* string);

#endif