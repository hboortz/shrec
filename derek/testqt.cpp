#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <glib.h>
#include "testqt.h"

static QString databuf;
QTextEdit *textEdit;

bool OnKeyPress::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    //QTextEdit *thisTextEdit = (QTextEdit*)obj;
    if (event->type() == QEvent::KeyPress) { //handle key press events
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event); //get the event object
        int key = keyEvent->key(); //extract key code (no upper/lower)
        qDebug("NVK: %d",keyEvent->nativeVirtualKey()); //extract and print NVK (useful because distinct upper/lower)
        if (key>=65 && key<=90){ //alpha key
            qDebug("Ate key press %d", key);
            qDebug("Character: %s", (char*)keyEvent->text().data());
            qDebug("Cursor position: %i\n",textEdit->textCursor().position());
            textEdit->textCursor().insertText(keyEvent->text()); //allow text insertion
            //databuf=textEdit->toPlainText(); //may not be necessary...
            //qDebug("%s\n",databuf);
            return true;
        } else {
            //test code: uncomment to insert A at the 
            //  start of the line on non-alpha keypress
            /*receiveEvent(0,65);
            return true;*/
            return false;
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
}

int receiveEvent(int pos, int event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor newcursor = textEdit->textCursor();
    QString text = QString(1,(char)event);
    newcursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(newcursor);
    textEdit->insertPlainText(text);
    textEdit->setTextCursor(oldcursor);
}

int sendEvent(int pos, int event){
    //method to be used to send an event to the server
}

int main(int argv, char **args){
    QApplication app(argv,args);
    databuf=QString();
    textEdit = new QTextEdit();
    OnKeyPress *onKeyPress = new OnKeyPress();
    textEdit->installEventFilter(onKeyPress);
    textEdit->show();
    

    return app.exec();
}