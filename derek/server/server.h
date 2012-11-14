#ifndef SERVER_H
#define SERVER_H

#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

class KeyPressListener: public QObject{
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);

#endif