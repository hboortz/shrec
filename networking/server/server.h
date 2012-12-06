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
#include <queue>
#include "../common.h"

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
  int receiveEvent(Event event);
  void broadcastAction(char *string);
  void initialWrite(QTcpSocket *client);
public slots:
  void acceptConnection();
  void startRead();
  void writeData();
  void saveData();
  void clientDisconnect();
private:
  QTcpServer server;
  QTcpSocket* client;
};

void executeEvent(int pos, QString string);
int sendEvent(int pos, int event);

#endif