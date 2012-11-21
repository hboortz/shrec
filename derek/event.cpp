#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct {
    int nvk;
    int pos;
} Event;

char *eventToString(Event event)
{
    char *pos = (char*)malloc(sizeof(char)*14);
    char *nvk = (char*)malloc(sizeof(char)*7);
    char *string = (char*)malloc(sizeof(char)*21);
    sprintf(pos,"%i",event.pos);
    sprintf(nvk,"%i",event.nvk);
    strcpy(string,pos);
    strcat(string,"|");
    strcat(string,nvk);
    printf("%s\n",string);
    return(string);
}

Event stringToEvent(char *string){
    int i = 0;
    int j;
    //changed values because buffer overflow on rapid keystrokes;
    //fix error and change back eventually
    char *pos = (char*)malloc(sizeof(char)*140); //used to be 14
    char *nvk = (char*)malloc(sizeof(char)*70); //used to be 7
    while(string[i] != '|'){
        i++;
    }
    strncpy(pos,string,i);
    pos[i]='\0';
    j=strlen(string)-i-1;
    strncpy(nvk,&string[i+1],j);
    nvk[j]='\0';
    Event event = {
        .nvk = atoi(nvk),
        .pos = atoi(pos)
    };
    return(event);
}