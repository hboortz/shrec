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

typedef struct {
    int nvk;
    int pos;
} Event;


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
  void connect_signal(void *ref);
public slots:
  void init();
  void startRead();
  void writeData(Event event);
private:
  QTcpSocket client;
};



char *eventToString(Event event);
int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);
void saveData();

#endif