#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QApplication>
#include <QtGui/QApplication>
#include <QTextEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class KeyPressListener: public QObject{
Q_OBJECT
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);
void saveData();

#endif