#include "common.h"

char *eventToString(Event event)
{
    char *pos = (char*)malloc(sizeof(char)*1000);
    char *nvk = (char*)malloc(sizeof(char)*6);
    char *string = (char*)malloc(sizeof(char)*MAX_MSG_SIZE);
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
    char *pos = (char*)malloc(sizeof(char)*MAX_MSG_SIZE*2/3); //used to be 14
    char *nvk = (char*)malloc(sizeof(char)*MAX_MSG_SIZE/3); //used to be 7
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

//prepends an Action code and a message length to string
//string format: ####|##|[msg]
// or: [4-char length]|[2-char action]|[msg]
void addMetadata(const Action action, char* string){
    char *newstring = (char*)malloc(sizeof(char)*MAX_MSG_SIZE);
    sprintf(newstring,"%.4i|%.2i|%s",static_cast<int>(strlen(string)),action,string);
    strcpy(string, newstring);
    free(newstring);
}

//returns the Action code at the start of inputString in action
//returns length of remaining message in size
//returns remaining message in msg
//modifies inputString in place to pop the size, action, and message from the string
//string format: ####|##|[msg]
void popMetadata(char **inputString, Action *action, int *size, char **msg){
    char *sizestring = (char*)malloc(sizeof(char)*5);
    char *actionstring = (char*)malloc(sizeof(char)*3);

    strncpy(sizestring,*inputString,4);
    sizestring[4]='\0';
    *size = atoi(sizestring);
    strncpy(actionstring,*inputString+5,2);
    actionstring[2]='\0';
    *action = static_cast<Action>(atoi(actionstring));
    printf("Size of message: %i\n",*size);
    
    *msg = (char*)malloc(sizeof(char)*(*size+5));
    strncpy(*msg,*inputString+8,*size);
    *(*msg+*size) = '\0';
    *inputString+=(*size+8);
    printf("Pre-return data %s\n",*msg);
}