#include "client.h"
#include <QClipboard>

QString databuf;
QString filename;
QTextEdit *editor;
MainWindow *window;
int *cursor_locked = (int*)malloc(sizeof(int));
QList<QTextEdit::ExtraSelection> cursorHighlights;
QList<QString> cursorIPs;


//----------------------------------------------------------

Client::Client(QObject* parent): QObject(parent)
{
    connect(&client, SIGNAL(connected()), this, SLOT(init()));
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(saveData()));
    timer->start(15000);
}

Client::~Client()
{
    client.close();
}

void Client::cursorPositionChanged() {
    if(!*cursor_locked) {
        Action action = CURSOR_MOVE;
        char *msg = (char*)malloc(sizeof(char*)*20);
        sprintf(msg,"%i|%i",editor->textCursor().position(),editor->textCursor().anchor());
        writeData(action,msg);
    }
}

void Client::connect_signal(ClientEventFilter *eventFilter, MainWindow *window){
    connect(eventFilter, SIGNAL(signalWrite(Action,char*)),this, SLOT(writeData(Action,char*)));
    connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(editor, SIGNAL(selectionChanged()), this, SLOT(cursorPositionChanged()));
    connect(window, SIGNAL(signalWrite(Action,char*)),this,SLOT(writeData(Action, char*)));
    connect(eventFilter, SIGNAL(cut()), window, SLOT(cut()));
    connect(eventFilter, SIGNAL(paste()), window, SLOT(paste()));
}

void Client::writeData(Action action, char *msg)
{
    //@puts("writing");
    char *stringToWrite = (char*)malloc(sizeof(char)*1024); //change size!
    strcpy(stringToWrite,msg);
    addMetadata(action,stringToWrite);
    //@printf("Sending to server: %s\n",stringToWrite);
    client.write(stringToWrite);
}

void Client::start(QString address, quint16 port)
{
    QHostAddress addr(address);
    client.connectToHost(addr, port);
}

void Client::quit() {
    puts("\nDisconnected from the server!");
    exit(-1);
}

void Client::init()
{
    connect(&client, SIGNAL(readyRead()), this, SLOT(startRead()));
    connect(&client, SIGNAL(disconnected()), this, SLOT(quit()));
    printf("connected to IP %s\n",client.peerAddress().toString().toLocal8Bit().data());
    editor->setPlainText("");
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(0,QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
}

void Client::startRead()
{
    //@puts("reading");
    char *buffer = (char*)malloc(sizeof(char)*10240);
    int bytesAvail = client.bytesAvailable();
    client.read(buffer, bytesAvail);
    buffer[bytesAvail] = '\0';
    int bytesPushed=0;
    int *size=(int*)malloc(sizeof(int));
    Action *action = (Action*)malloc(sizeof(int));
    char *msg;

    //loop through the available messages
    while(bytesPushed<bytesAvail){
    *size=0;
    popMetadata(&buffer,action,size,&msg);
    bytesPushed+=(*size+8);
    if(bytesPushed<=bytesAvail){

        //@printf("Bytes pushed: %i, Bytes available: %i\n",bytesPushed,bytesAvail);
        if(bytesPushed<bytesAvail){
            //@printf("Msg: %s\nData remaining: %s\n\n",msg,buffer);
        } else {
            //@printf("Msg: %s\n\n",msg);
        }
        switch(*action) {
            case KEY_EVENT:
                receiveEvent(stringToEvent(msg));
                break;
            case INSERT_STRING:
                insertString(msg);
                break;
            case REMOVE_STRING:
                removeString(msg);
                break;
            case INITIAL_SEND:
                initialRead(msg);
                break;
            case CURSOR_MOVE:
                if(moveRemoteCursor(msg)) cursorPositionChanged();
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

void Client::initialRead(char *msg){
    char *newtext = (char*)malloc(strlen(msg)+strlen(editor->toPlainText().toLocal8Bit().data()));
    strcpy(newtext,editor->toPlainText().toLocal8Bit().data());
    strcat(newtext,msg);
    //@printf("Part of the initial read: %s\n",msg);
    editor->setPlainText(newtext);
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
    } else if (event.nvk==65289) { //tab
        text = QString("    ");
    } else {
        text = QString("");
    }
    executeEvent(event.pos,text);
    return(0);
}

void Client::saveData(){
    QString contents = QString(editor->toPlainText());
    FILE *file;

    window->updateFileName();
    filename=QString(window->fileName);
    //puts(filename.toLocal8Bit().data());

    file = fopen(filename.toLocal8Bit().data(),"w+");
    QFile qfile;
    qfile.open(file,QIODevice::WriteOnly);
    qfile.write(contents.toLocal8Bit().data());
    qfile.close();
    fclose(file);
}

//---------------------------------------------------------------

//returns 1 if this is a new cursor; 0 otherwise
int moveRemoteCursor(char *msg) {
    char *ip = (char*)malloc(sizeof(char)*20);
    char *tempstring = (char*)malloc(sizeof(char)*20);
    char *posstring = (char*)malloc(sizeof(char)*10);
    char *anchorstring = (char*)malloc(sizeof(char)*10);
    int size=strlen(msg);
    int pos;
    int anchor;
    int ipsize = strchr(msg,'|') - msg;
    strncpy(ip,msg,ipsize);
    ip[ipsize]='\0';
    strcpy(tempstring,msg+ipsize+1);
    int possize = strchr(tempstring,'|') - tempstring;
    strncpy(posstring,tempstring,possize);
    posstring[possize]='\0';
    strcpy(anchorstring,tempstring+possize+1);
    anchorstring[size-possize-ipsize-2]='\0';
    pos=atoi(posstring);
    anchor=atoi(anchorstring);

    int found=0;
    int i;

    for(i=0;i<cursorIPs.size();i++) {
        if(cursorIPs.at(i)==QString(ip)) {
            found=1;
            if(pos==-1){
                cursorIPs.removeAt(i);
                cursorHighlights.removeAt(i);
            } else {
                cursorHighlights[i].cursor.setPosition(pos,QTextCursor::MoveAnchor);
                if(pos==anchor){
                    cursorHighlights[i].cursor.setPosition(pos+1,QTextCursor::KeepAnchor);
                } else {
                    cursorHighlights[i].cursor.setPosition(anchor,QTextCursor::KeepAnchor);
                }
            }
        }
    }

    if(!found) {
        //add a new client
        QTextEdit::ExtraSelection highlight;
        highlight.cursor = editor->textCursor();
        highlight.cursor.setPosition(pos,QTextCursor::MoveAnchor);
        highlight.cursor.setPosition(pos+1,QTextCursor::KeepAnchor);
        highlight.format = editor->currentCharFormat();
        highlight.format.setBackground(pickColor(ip));
        cursorIPs.append(ip);
        cursorHighlights.append(highlight);
    }

    editor->setExtraSelections(cursorHighlights);

    return(!found);
}

QColor pickColor(char *ip) {
    int len = strlen(ip);
    int i;
    int total=0;
    for(i=0;i<len;i++) {
        total+=ip[i];
    }
    printf("total: %i\n",total);
    switch(total%6) {
        case '0': return QColor::fromRgb(40,60,80);
        case '1': return QColor::fromRgb(60,40,80);
        case '2': return QColor::fromRgb(40,80,60);
        case '3': return QColor::fromRgb(60,80,40);
        case '4': return QColor::fromRgb(80,40,60);
        case '5': return QColor::fromRgb(80,60,40);
        default: return QColor::fromRgb(40,60,80);
    }
}

void removeString(char *msg) {
    char *posstring = (char*)malloc(sizeof(char)*10);
    char *anchorstring = (char*)malloc(sizeof(char)*10);
    int pos;
    int anchor;
    int possize = strchr(msg,'|') - msg;
    strncpy(posstring,msg,possize);
    strcpy(anchorstring,msg+possize+1);
    pos=atoi(posstring);
    anchor=atoi(anchorstring);

    if(*cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    *cursor_locked=1;
    QTextCursor oldcursor = editor->textCursor();
    QTextCursor tempcursor = editor->textCursor();
    tempcursor.setPosition(anchor,QTextCursor::MoveAnchor);
    tempcursor.setPosition(pos,QTextCursor::KeepAnchor);
    editor->setTextCursor(tempcursor);
    tempcursor.deleteChar();
    editor->setTextCursor(oldcursor);
    *cursor_locked=0;
}

void insertString(char *msg) {
    char *posstring = (char*)malloc(sizeof(char)*10);
    char *insertstring = (char*)malloc(sizeof(char)*1024);
    int pos;
    int possize = strchr(msg,'|') - msg;
    strncpy(posstring,msg,possize);
    strcpy(insertstring,msg+possize+1);
    pos=atoi(posstring);

    if(*cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    *cursor_locked=1;
    QTextCursor oldcursor = editor->textCursor();
    QTextCursor tempcursor = editor->textCursor();
    tempcursor.setPosition(pos,QTextCursor::MoveAnchor);
    editor->setTextCursor(tempcursor);
    editor->insertPlainText(insertstring);
    editor->setTextCursor(oldcursor);
    *cursor_locked=0;
}

void executeEvent(int pos, QString string){
    if(*cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    *cursor_locked=1;
    QTextCursor oldcursor = editor->textCursor();
    QTextCursor tempcursor = editor->textCursor();
    tempcursor.setPosition(pos,QTextCursor::MoveAnchor);
    editor->setTextCursor(tempcursor);
    if ((string == "bksp")||(string == "del")) {
        puts("\n\nThat shouldn't happen here!\n");
    } else {
        editor->insertPlainText(string);
    }
    editor->setTextCursor(oldcursor);
    *cursor_locked=0;
}

int main(int argv, char **args){
    //filename=QString("test.txt");
    *cursor_locked=0;
    QApplication app(argv,args);
    Client client;
    window = new MainWindow();
    window->updateFileName();
    filename=QString(window->fileName);
    //puts(filename.toLocal8Bit().data());

    if (argv<=1) {
        client.start("127.0.0.1", 8888);
    } else {
        client.start(args[1], 8888);
    }
    databuf=QString();
    editor = new QTextEdit();

    editor = window->editor;
    //printf("Editor: %p, window.editor: %p\n",editor,(window.editor));
    ClientEventFilter *eventFilter = new ClientEventFilter(editor,cursor_locked);
    client.connect_signal(eventFilter,window);

    editor->installEventFilter(eventFilter);
    window->show();
    //textEdit->show();

    return app.exec();
}
