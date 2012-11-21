#include "server.h"
using namespace std;

static QString databuf;
QTextEdit *textEdit;
QString filename;
QTcpSocket *clients[5];
int numclients;

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
  clients[numclients] = server.nextPendingConnection();
  puts("connected");
  connect(clients[numclients], SIGNAL(readyRead()),
    this, SLOT(startRead()));
  numclients+=1;
}

void Server::startRead()
{
  QTcpSocket *readClient = qobject_cast<QTcpSocket *>(sender());
  puts("reading");
  char buffer[10240] = {0};
  readClient->read(buffer, readClient->bytesAvailable());
  printf("%s\n",buffer);
  receiveEvent(stringToEvent(buffer));
  cout << buffer << endl;
  //client->close();
}

void Server::broadcastEvent(Event event)
{
    int i;
    char *string = eventToString(event);
    for(i=0;i<numclients;i++){
        clients[i]->write(string);
    }
}

int Server::receiveEvent(Event event){
    //called when the app receives an event from the server
    //returns 0 for no error
    QString text;
    if (event.nvk>=33 && event.nvk<=126){
        text = QString(1,(char)event.nvk);
    } else if (event.nvk==65293) { //enter
        text = QString("\n");
    } else if (event.nvk==65288) { //backspace
        text = QString("bksp");
    } else if (event.nvk==65535) { //delete
        text = QString("del");
    } else {
        text = QString("");
    }
    broadcastEvent(event);
    executeEvent(event.pos,text);
    return(0);
}

void executeEvent(int pos, QString string){
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(cursor);
    if (string == "bksp") {
        cursor.deletePreviousChar();
    } else if (string == "del") {
        cursor.deleteChar();
    } else {
        textEdit->insertPlainText(string);
    }
    saveData();
}

char *eventToString(Event event)
{
    char *pos = (char*)malloc(sizeof(char)*14);
    char *nvk = (char*)malloc(sizeof(char)*7);
    char *string = (char*)malloc(sizeof(char)*21);
    sprintf(pos,"%i",event.pos);
    sprintf(nvk,"%i",event.nvk);
    strcpy(string,pos);
    strcat(string,"|");
    strcat(string,nvk);
    printf("%s\n",string);
    return(string);
}

Event stringToEvent(char *string){
    int i = 0;
    int j;
    //changed values because buffer overflow on rapid keystrokes;
    //fix error and change back eventually
    char *pos = (char*)malloc(sizeof(char)*140); //used to be 14
    char *nvk = (char*)malloc(sizeof(char)*70); //used to be 7
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
        return true;
    }
    return false; //server cannot interact with code
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
    numclients=0;
    QApplication app(argv,args);
    stringToEvent(const_cast<char *>("5345987652|1461"));
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