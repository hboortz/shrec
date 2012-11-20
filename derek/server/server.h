#ifndef SERVER_H
#define SERVER_H

#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui/QKeyEvent>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct {
    int nvk;
    int pos;
} Event;

class KeyPressListener: public QObject{
Q_OBJECT
public:
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    void signalWrite();
};

class Server: public QObject
{
Q_OBJECT
public:
  Server(QObject * parent = 0);
  ~Server();
  void connect_signal(void *ref);
public slots:
  void acceptConnection();
  void startRead();
  void writeData();
private:
  QTcpServer server;
  QTcpSocket* client;
};


void executeEvent(int pos, QString string);
Event stringToEvent(char *string);
int receiveEvent(Event event);
int sendEvent(int pos, int event);
void saveData();

#endif