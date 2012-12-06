#include "server.h"
using namespace std;

static QString databuf;
QTextEdit *textEdit;
QString filename;
QTcpSocket *clients[10];
int numclients;

//----------------------------------------------------------------

Server::Server(QObject* parent): QObject(parent)
{
    connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

    server.listen(QHostAddress::Any, 8888);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(saveData()));
    timer->start(15000);
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
    connect(clients[numclients], SIGNAL(readyRead()), this, SLOT(startRead()));
    connect(clients[numclients], SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    initialWrite(clients[numclients]);
    numclients+=1;
}

void Server::clientDisconnect() {
    numclients--;
}

void Server::initialWrite(QTcpSocket *client)
{
    char *text = textEdit->toPlainText().toLocal8Bit().data();
    char *temp = (char*)malloc(sizeof(char)*1024);
    int length = strlen(text);
    Action action = INITIAL_SEND;
    while(length>0){
        strncpy(temp,text,1015);
        temp[1015] = '\0';
        addMetadata(action,temp);
        client->write(temp);
        length-=1015;
        text+=1015;
    }
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
            case ADD_STRING:
                break;
            case REMOVE_STRING:
                break;
            case INITIAL_SEND:
                puts("That's weird. Clients should never send that action.");
                exit(-1);
                break;
            case CURSOR_MOVE:
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

void Server::saveData(){
    puts("save");
    QString contents = QString(textEdit->toPlainText());
    FILE *file;
    file = fopen(filename.toLocal8Bit().data(),"w+");
    QFile qfile;
    qfile.open(file,QIODevice::WriteOnly);
    qfile.write(contents.toLocal8Bit().data());
    qfile.close();
    fclose(file);
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
}

//-----------------------------------------------------------------

//NOTE: This code has the potential to intercept possibly useful events!
//  Change if necessary.
bool KeyPressListener::eventFilter(QObject *obj, QEvent *event){
    //called when textEdit has an event
    switch(event->type()){
        case QEvent::UpdateRequest: return false;
        case QEvent::Paint: return false;
        default: return true;
    }
}

int main(int argv, char **args){
    numclients=0;
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