#include "clienteventfilter.h"

ClientEventFilter::ClientEventFilter(QTextEdit *inputEditor) {
    editor = inputEditor;
}

bool ClientEventFilter::eventFilter(QObject *obj, QEvent *event){
    //called when editor has an event
    if (event->type() == QEvent::KeyPress) { //handle key press events
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event); //get the event object
        int nvk = keyEvent->nativeVirtualKey(); //extract NVK (useful because distinct upper/lower values)
        int pos = editor->textCursor().position();
        qDebug("NVK: %d",nvk);
        if ((nvk>=32 && nvk<=126)||(nvk==65289)||(nvk==65293)) {
            Event textevent = {
                .nvk = nvk,
                .pos = pos
            };
            Action action = KEY_EVENT;
            emit signalWrite(action, eventToString(textevent));
        } else if ((nvk==65288)||(nvk==65535)) { //backspace or delete
            Action action = REMOVE_STRING;
            char *msg = (char*)malloc(sizeof(char)*20);
            QTextCursor cursor = editor->textCursor();
            int position = cursor.position();
            int anchor = cursor.anchor();
            if((nvk==65288 && position==0)||(nvk==65535 && position==static_cast<int>(strlen(editor->toPlainText().toLocal8Bit())))) {
                return true; //bksp at pos 0 or del at pos max doesn't do anything
            }
            if(position==anchor) {
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
            emit signalWrite(action, msg);
        } else if ((nvk>=65360)&&(nvk<=65367)) {
            return false; //navigation
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}