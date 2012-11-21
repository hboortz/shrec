#include "client.h"
#include <QtGui/QApplication>

QString databuf;
QString filename;
QTextEdit *textEdit;
Client client;


//----------------------------------------------------------

Client::Client(QObject* parent): QObject(parent)
{
  connect(&client, SIGNAL(connected()),
    this, SLOT(init()));
}

Client::~Client()
{
  client.close();
}

void Client::connect_signal(void *ref){
    connect((KeyPressListener*)ref, SIGNAL(signalWrite(Event)),this, SLOT(writeData(Event)));
}

void Client::writeData(Event event)
{
    puts("writing");
    char *eventString = eventToString(event);
    client.write(eventString);
}

void Client::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  client.connectToHost(addr, port);
}

void Client::init()
{
  //client.write("Hello, world", 13);
    connect(&client, SIGNAL(readyRead()), this, SLOT(startRead()));
    puts("connected");
}

void Client::startRead()
{
  char buffer[1024] = {0};
  client.read(buffer, client.bytesAvailable());
  receiveEvent(stringToEvent(buffer));
  printf("%s\n",buffer);
}

int Client::receiveEvent(Event event){
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
    executeEvent(event.pos,text);
    return(0);
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

//---------------------------------------------------------------

bool KeyPressListener::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    //QTextEdit *thisTextEdit = (QTextEdit*)obj;
    if (event->type() == QEvent::KeyPress) { //handle key press events
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event); //get the event object
        int key = keyEvent->key(); //extract key code (no upper/lower)
        int nvk = keyEvent->nativeVirtualKey(); //extract NVK (useful because distinct upper/lower values)
        int pos = textEdit->textCursor().position();
        qDebug("NVK: %d",nvk);
        Event event = {
            .nvk = nvk,
            .pos = pos
        };
        emit signalWrite(event);
        if (nvk>=65361 && nvk<=65364){
            return false;
        } else {
            return true;
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
}

void executeEvent(int pos, QString string){
    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor tempcursor = textEdit->textCursor();
    tempcursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(tempcursor);
    if (string == "bksp") {
        tempcursor.deletePreviousChar();
    } else if (string == "del") {
        tempcursor.deleteChar();
    } else {
        textEdit->insertPlainText(string);
    }
    textEdit->setTextCursor(oldcursor);
    saveData();
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
    Event event = {
        .nvk = 5,
        .pos = 23
    };
    //event->nvk=5;
    //event->pos=23;
    eventToString(event);

    filename=QString("test.txt");
    QApplication app(argv,args);
    client.start("127.0.0.1", 8888);
    databuf=QString();
    textEdit = new QTextEdit();
    KeyPressListener *keylisten = new KeyPressListener();
    client.connect_signal(keylisten);
    textEdit->installEventFilter(keylisten);
    textEdit->show();
    
    return app.exec();
}