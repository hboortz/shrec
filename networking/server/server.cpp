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
    char *buffer = (char*)malloc(sizeof(char)*10240);
    int bytesAvail = readClient->bytesAvailable();
    readClient->read(buffer, bytesAvail);
    buffer[bytesAvail] = '\0';
    printf("%s\n",buffer);
    int bytesPushed=0;
    int *size=(int*)malloc(sizeof(int));
    Action *action = (Action*)malloc(sizeof(int));
    char *msg;

    //loop through the available messages
    while(bytesPushed<bytesAvail){
    *size=0;
    popMetadata(&buffer,action,size,&msg);
    printf("Msg: %s\n",msg);
    bytesPushed+=(*size+8);
    if(bytesPushed<=bytesAvail){
        
        printf("Bytes pushed: %i, Bytes available: %i\n",bytesPushed,bytesAvail);
        if(bytesPushed<bytesAvail){
            printf("Msg: %s\nData remaining: %s\n",msg,buffer);
        } else {
            printf("Msg: %s\n",msg);
        }
        switch(*action) {
            case KEY_EVENT:
                receiveEvent(stringToEvent(msg));
                break;
            default:
                puts("We don't take your kind here.");
                break;
        }
        addMetadata(*action,msg);
        printf("Sending: %s\n",msg);
        broadcastAction(msg);
    } else {
        puts("Well, that went badly.");
    }
    }

    //make 2 readQueue s
    //make a readQueue execution loop

    //receiveEvent(stringToEvent(buffer));
    //cout << buffer << endl;
    //client->close();
}

void Server::broadcastAction(char *string)
{
    int i;
    //char *string = eventToString(event);
    for(i=0;i<numclients;i++){
        clients[i]->write(string);
    }
}

int Server::receiveEvent(Event event){
    //called when the app receives an event from the server
    //returns 0 for no error
    QString text;
    if (event.nvk>=32 && event.nvk<=126){
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
    //broadcastAction(eventToString(event));
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