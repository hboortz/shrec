#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <QDebug>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <glib.h>
#include "client.h"

static QString databuf;
QString filename;
QTextEdit *textEdit;

bool OnKeyPress::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    //QTextEdit *thisTextEdit = (QTextEdit*)obj;
    if (event->type() == QEvent::KeyPress) { //handle key press events
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event); //get the event object
        int key = keyEvent->key(); //extract key code (no upper/lower)
        int nvk = keyEvent->nativeVirtualKey(); //extract NVK (useful because distinct upper/lower values)
        int pos = textEdit->textCursor().position();
        qDebug("NVK: %d",nvk);
        if (key>=65 && key<=90){ //alpha key
            receiveEvent(pos,nvk);
            qDebug("Ate key press %d", key);
            qDebug("Character: %s", (char*)keyEvent->text().data());
            qDebug("Cursor position: %i\n",pos);
            //textEdit->textCursor().insertText(keyEvent->text()); //allow text insertion
            //databuf=textEdit->toPlainText(); //may not be necessary...
            //qDebug() << databuf;
            saveData();
            return true;
        } else {
            //test code: uncomment to insert A at the 
            //  start of the line on non-alpha keypress
            /*receiveEvent(0,65);
            return true;*/
            saveData();
            return false;
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
}

void saveData(){
    qDebug("save");
    QString contents = QString(textEdit->toPlainText());
    FILE *file;
    file = fopen(filename.toLocal8Bit().data(),"w+");
    QFile qfile;
    qfile.open(file,QIODevice::WriteOnly);
    qfile.write(contents.toLocal8Bit().data());
    qfile.close();
    fclose(file);
}

int receiveEvent(int pos, int event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    //returns 0 for no error
    qDebug("update");
    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor newcursor = textEdit->textCursor();
    QString text = QString(1,(char)event);
    newcursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(newcursor);
    textEdit->insertPlainText(text);
    textEdit->setTextCursor(oldcursor);
    saveData();
    return(0);
}

int sendEvent(int pos, int event){
    //method to be used to send an event to the server
    //returns 0 for no error
    return(0);
}

int main(int argv, char **args){
    filename=QString("test.txt");
    QApplication app(argv,args);
    databuf=QString();
    textEdit = new QTextEdit();
    OnKeyPress *onKeyPress = new OnKeyPress();
    textEdit->installEventFilter(onKeyPress);
    textEdit->show();
    

    return app.exec();
}