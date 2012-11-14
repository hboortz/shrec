/// client.h
#ifndef NET_CLIENT_H
#define NET_CLIENT_H
#include <QObject>
#include <QtGui/QApplication>
#include <QtNetwork>
#include <QString>
#include <QTcpSocket>

class Client: public QObject
{
Q_OBJECT

public:
  Client(QObject* parent = 0);
  ~Client();
  void start(QString address, quint16 port);
public slots:
  void startTransfer();
private:
  QTcpSocket client;
};


#endif