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
  receiveEvent(stringToEvent(buffer));
  cout << buffer << endl;
  //client->close();
}

Event stringToEvent(char *string){
    int i = 0;
    int j;
    char *pos = (char*)malloc(sizeof(char)*14);
    char *nvk = (char*)malloc(sizeof(char)*7);
    while(string[i] != '|'){
        i++;
    }
    strncpy(pos,string,i);
    pos[i]='\0';
    j=strlen(string)-i-1;
    strncpy(nvk,&string[i+1],j);
    nvk[j]='\0';
    Event event = {
        .nvk = atoi(nvk),
        .pos = atoi(pos)
    };
    return(event);
}

//-----------------------------------------------------------------

bool KeyPressListener::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    if (event->type() == QEvent::KeyPress) {
        //emit signalWrite();
    }
    return false; //server cannot interact with code
}

int receiveEvent(Event event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    //returns 0 for no error
    QTextCursor cursor = textEdit->textCursor();
    QString text = QString(1,(char)event.nvk);
    //QString text = QString("a");
    cursor.setPosition(event.pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(cursor);
    textEdit->insertPlainText(text);
    printf("%s",text.data());
    //saveData();
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
    stringToEvent("5345987652|1461");
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