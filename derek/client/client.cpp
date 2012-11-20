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
  printf("%s\n",buffer);
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
        if (key>=65 && key<=90){ //alpha key
            Event event = {
                .nvk = nvk,
                .pos = pos
            };
            emit signalWrite(event);
            receiveEvent(pos,nvk);
            qDebug("Ate key press %d", key);
            qDebug("Character: %s", (char*)keyEvent->text().data());
            qDebug("Cursor position: %i\n",pos);
            //textEdit->textCursor().insertText(keyEvent->text()); //allow text insertion
            //databuf=textEdit->toPlainText(); //may not be necessary...
            //qDebug() << databuf;
            saveData();
            return true;
        } else {
            //test code: uncomment to insert A at the 
            //  start of the line on non-alpha keypress
            //receiveEvent(0,65);
            //return true;
            saveData();
            return false;
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
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

int receiveEvent(int pos, int event){
    //called when the app receives an event from the server
    //currently only inserts characters using event as the char code
    //returns 0 for no error
    qDebug("update");
    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor newcursor = textEdit->textCursor();
    QString text = QString(1,(char)event);
    newcursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(newcursor);
    textEdit->insertPlainText(text);
    textEdit->setTextCursor(oldcursor);
    saveData();
    return(0);
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