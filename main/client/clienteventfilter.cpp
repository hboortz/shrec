#include "clienteventfilter.h"

ClientEventFilter::ClientEventFilter(QTextEdit *inputEditor, int *input_cursor_locked) {
    editor = inputEditor;
    cursor_locked = input_cursor_locked;
}

bool ClientEventFilter::eventFilter(QObject *obj, QEvent *event){
    //called when editor has an event
    if (event->type() == QEvent::KeyPress) { //handle key press events
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event); //get the event object
        if(keyEvent->isAutoRepeat()) return true; //disable keypress repeats (when user is holding down a key)
        int nvk = keyEvent->nativeVirtualKey(); //extract NVK (useful because distinct upper/lower values)
        int pos = editor->textCursor().position();
        //@printf("NVK: %d",nvk);
        int modifiers = keyEvent->nativeModifiers();
        //@printf("Modifiers: %i\n",modifiers);
        if (nvk==99 && modifiers&4) { //because ctrl is determined by 3rd bit
            puts("Copy");
            return false;
        }
        if (nvk==120 && modifiers&4) {
            puts("Cut");
            return true;
        }
        if (nvk==118 && modifiers&4) {
            puts("Paste");
            return true;
        }
        if ((nvk>=32 && nvk<=126)||(nvk==65289)||(nvk==65293)) {
            Event textevent = {
                .nvk = nvk,
                .pos = pos
            };

            if(*cursor_locked){
                exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
            }
            *cursor_locked=1;

            QTextCursor cursor = editor->textCursor();
            int position = cursor.position();
            int anchor = cursor.anchor();
            Action action;
            if(position!=anchor) {
                //overwrite text by deleting it first
                action=REMOVE_STRING;
                char *msg = (char*)malloc(sizeof(char)*20);
                sprintf(msg,"%i|%i",position,anchor);
                emit signalWrite(action, msg);
                if(position>anchor) {
                    textevent.pos=anchor;
                }
            }
                action = KEY_EVENT;
                emit signalWrite(action, eventToString(textevent));
            
            *cursor_locked=0;
        } else if ((nvk==65288)||(nvk==65535)) { //backspace or delete
            Action action = REMOVE_STRING;
            char *msg = (char*)malloc(sizeof(char)*20);

            if(*cursor_locked){
                exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
            }
            *cursor_locked=1;

            QTextCursor cursor = editor->textCursor();
            int position = cursor.position();
            int anchor = cursor.anchor();
            if(position==anchor) {
                if((nvk==65288 && position==0)||(nvk==65535 && position==static_cast<int>(strlen(editor->toPlainText().toLocal8Bit())))) {
                    return true; //bksp at pos 0 or del at pos max doesn't do anything
                }
                if(nvk==65288){ //bksp
                    sprintf(msg,"%i|%i",position-1,position);
                } else {
                    sprintf(msg,"%i|%i",position,position+1);
                }
            } else if(position<anchor) {
                sprintf(msg,"%i|%i",position,anchor);
            } else {
                sprintf(msg,"%i|%i",anchor,position);
            }
            *cursor_locked=0;
            emit signalWrite(action, msg);
        } else if ((nvk>=65360)&&(nvk<=65367)) {
            return false; //navigation
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}