#ifndef CLIENT_H
#define CLIENT_H

#include "../common.h"
#include "clienteventfilter.h"
#include "mainwindow.h"
#include "highlighter.h"

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
  void writeData(Action action, char *msg);
  void cursorPositionChanged();
  void saveData();
private:
  QTcpSocket client;
};

/*class ClientEventFilter : public QObject {
Q_OBJECT
public:
  bool eventFilter(QObject *obj, QEvent *event);
signals:
  void signalWrite(Action action, char *msg);
};*/


void executeEvent(int pos, QString string);
int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);
void removeString(char *msg);

#endif