#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "testqt.h"

bool OnKeyPress::eventFilter(QObject *obj, QEvent *event){
    std::cout << "ohai!" << std::endl;
    if (event->type() == QEvent::KeyPress) {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
         qDebug("Ate key press %d", keyEvent->key());
         return true;
     } else {
         // standard event processing
         //return QObject::eventFilter(obj, event);
        return false;
     }
}

int main(int argv, char **args){
    QApplication app(argv,args);

    QTextEdit *textEdit = new QTextEdit();
    OnKeyPress *onKeyPress = new OnKeyPress();
    textEdit->installEventFilter(onKeyPress);
    textEdit->show();
    

    return app.exec();
}