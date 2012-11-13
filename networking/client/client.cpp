//connectToHost
//check is connected
//send data
//close
//
//
//From: http://www.qtforum.org/article/26552/socket-communication-with-tcp-client-server-sample-code.html
//
// client.cc
#include "client.h"
#include <QHostAddress>

Client::Client(QObject* parent): QObject(parent)
{
  connect(&client, SIGNAL(connected()),
    this, SLOT(startTransfer()));
}

Client::~Client()
{
  client.close();
}

void Client::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  client.connectToHost(addr, port);
}

void Client::startTransfer()
{
  client.write("Hello, world", 13);
}

// main.cc
#include "client.h"
#include <QApplication>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Client client;
  client.start("127.0.0.1", 8888);

  return app.exec();
}
