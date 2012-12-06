#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QString>
#include <QTcpSocket>
#include <QApplication>
#include <QtGui/QApplication>
#include <QTextEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QHostAddress>
#include "../common.h"


class KeyPressListener: public QObject{
Q_OBJECT
public:
  bool eventFilter(QObject *obj, QEvent *event);
signals:
  void signalWrite(Event event);
};


class Client: public QObject
{
Q_OBJECT

public:
  Client(QObject* parent = 0);
  ~Client();
  void start(QString address, quint16 port);
  void connect_signal(void *ref1, void *ref2);
  int receiveEvent(Event event);
  void initialRead(char *msg);
public slots:
  void init();
  void startRead();
  void writeData(Event event);
  void cursorPositionChanged();
  void saveData();
private:
  QTcpSocket client;
};


void executeEvent(int pos, QString string);
int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);

#endif