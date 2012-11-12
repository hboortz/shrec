#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <glib.h>
#include "server.h"

static QString databuf;
QTextEdit *textEdit;

bool OnKeyPress::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    return true; //server cannot interact with code
}

int receiveEvent(int pos, int event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    //returns 0 for no error
    QTextCursor cursor = textEdit->textCursor();
    QString text = QString(1,(char)event);
    cursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(cursor);
    textEdit->insertPlainText(text);
    return(0);
}

int sendEvent(int pos, int event){
    //method to be used to send an event to the server
    //returns 0 for no error
    return(0);
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