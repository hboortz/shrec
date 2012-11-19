#include "server.h"
using namespace std;

static QString databuf;
QTextEdit *textEdit;
QString filename;

//----------------------------------------------------------------

Server::Server(QObject* parent): QObject(parent)
{
  connect(&server, SIGNAL(newConnection()),
    this, SLOT(acceptConnection()));

  server.listen(QHostAddress::Any, 8888);
}

void Server::connect_signal(void *ref){
    connect((KeyPressListener*)ref, SIGNAL(signalWrite()),this, SLOT(writeData()));
}

Server::~Server()
{
  server.close();
}

void Server::writeData()
{
    puts("writing");
    if(client){
        client->write("writing");
    }
}

void Server::acceptConnection()
{
  client = server.nextPendingConnection();
  puts("connected");
  connect(client, SIGNAL(readyRead()),
    this, SLOT(startRead()));
}

void Server::startRead()
{
  char buffer[1024] = {0};
  client->read(buffer, client->bytesAvailable());
  cout << buffer << endl;
  //client->close();
}

//-----------------------------------------------------------------

bool KeyPressListener::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    if (event->type() == QEvent::KeyPress) {
        //emit signalWrite();
    }
    return true; //server cannot interact with code
}

int receiveEvent(int pos, int event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    //returns 0 for no error
    QTextCursor cursor = textEdit->textCursor();
    QString text = QString(1,(char)event);
    cursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(cursor);
    textEdit->insertPlainText(text);
    saveData();
    return(0);
}

void saveData(){
    qDebug("save");
    QString contents = QString(textEdit->toPlainText());
    FILE *file;
    file = fopen(filename.toLocal8Bit().data(),"w+");
    QFile qfile;
    qfile.open(file,QIODevice::WriteOnly);
    qfile.write(contents.toLocal8Bit().data());
    qfile.close();
    fclose(file);
}

int sendEvent(int pos, int event){
    //method to be used to send an event to the server
    //returns 0 for no error
    return(0);
}

int main(int argv, char **args){
    QApplication app(argv,args);
    filename=QString("test.txt");
    databuf=QString();
    textEdit = new QTextEdit();
    KeyPressListener *keylisten = new KeyPressListener();
    Server server;
    server.connect_signal(keylisten);
    textEdit->installEventFilter(keylisten);
    textEdit->show();

    return app.exec();
}