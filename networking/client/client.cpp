#include "client.h"
#include <QtGui/QApplication>

QString databuf;
QString filename;
QTextEdit *textEdit;
Client client;
int cursor_locked = 0;


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

void Client::cursorPositionChanged() {
    if(!cursor_locked) {
        puts("Position changed.");
    }
}

void Client::connect_signal(void *ref1, void *ref2){
    connect((KeyPressListener*)ref1, SIGNAL(signalWrite(Event)),this, SLOT(writeData(Event)));
    connect((QTextEdit*)ref2, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
}

void Client::writeData(Event event)
{
    puts("writing");
    char *eventString = (char*)malloc(sizeof(char)*1024); //change size!
    strcpy(eventString,eventToString(event));
    Action action = KEY_EVENT;
    addMetadata(action,eventString);
    printf("%s\n",eventString);
    client.write(eventString);
    //client.waitForBytesWritten();
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
    puts("reading");
    char *buffer = (char*)malloc(sizeof(char)*10240);
    int bytesAvail = client.bytesAvailable();
    client.read(buffer, bytesAvail);
    buffer[bytesAvail] = '\0';
    printf("Buffer: %s\n",buffer);
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
    } else {
        puts("Well, that went badly.");
    }
    }
  
}

int Client::receiveEvent(Event event){
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
    executeEvent(event.pos,text);
    return(0);
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
    cursor_locked=1;
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

    //Experimental block of code concerning text highlighting
    //----------------------------------------------------
    QTextEdit::ExtraSelection highlight;
    highlight.cursor = textEdit->textCursor();
    highlight.cursor.setPosition(pos,QTextCursor::MoveAnchor);
    highlight.cursor.setPosition(pos+1,QTextCursor::KeepAnchor);
    highlight.format = textEdit->currentCharFormat();
    if(pos%2==0){
        highlight.format.setBackground(Qt::green);
    }else{
        highlight.format.setBackground(Qt::red);
    }
    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    textEdit->setExtraSelections( extras );
    //------------------------------------------------------

    saveData();
    cursor_locked=0;
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
    if (argv<=1) {
        client.start("127.0.0.1", 8888);
    } else {
        client.start(args[1], 8888);
    }
    databuf=QString();
    textEdit = new QTextEdit();
    KeyPressListener *keylisten = new KeyPressListener();
    client.connect_signal(keylisten,textEdit);
    textEdit->installEventFilter(keylisten);
    textEdit->show();
    
    return app.exec();
}