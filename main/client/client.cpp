#include "client.h"
#include <QClipboard>

QString databuf;
QString filename;
QTextEdit *editor;
MainWindow *window;
int cursor_locked = 0;


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
    if(!cursor_locked) {
        puts("Position changed.");
    }
    }

void Client::connect_signal(void *ref1, void *ref2){
    connect((ClientEventFilter*)ref1, SIGNAL(signalWrite(Action,char*)),this, SLOT(writeData(Action,char*)));
    connect((QTextEdit*)ref2, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    puts("test");
}

void Client::writeData(Action action, char *msg)
{
    puts("writing");
    char *stringToWrite = (char*)malloc(sizeof(char)*1024); //change size!
    strcpy(stringToWrite,msg);
    addMetadata(action,stringToWrite);
    printf("Sending to server: %s\n",stringToWrite);
    client.write(stringToWrite);
}

void Client::start(QString address, quint16 port)
{
    QHostAddress addr(address);
    client.connectToHost(addr, port);
}

void Client::init()
{
    connect(&client, SIGNAL(readyRead()), this, SLOT(startRead()));
    puts("connected");
    editor->setPlainText("");
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
    bytesPushed+=(*size+8);
    if(bytesPushed<=bytesAvail){

        printf("Bytes pushed: %i, Bytes available: %i\n",bytesPushed,bytesAvail);
        if(bytesPushed<bytesAvail){
            printf("Msg: %s\nData remaining: %s\n\n",msg,buffer);
        } else {
            printf("Msg: %s\n\n",msg);
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
    printf("Part of the initial read: %s\n",msg);
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
    puts("save");
    QString contents = QString(editor->toPlainText());
    FILE *file;
    file = fopen(filename.toLocal8Bit().data(),"w+");
    QFile qfile;
    qfile.open(file,QIODevice::WriteOnly);
    qfile.write(contents.toLocal8Bit().data());
    qfile.close();
    fclose(file);
}

//---------------------------------------------------------------

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

    if(cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    cursor_locked=1;
    QTextCursor oldcursor = editor->textCursor();
    QTextCursor tempcursor = editor->textCursor();
    tempcursor.setPosition(anchor,QTextCursor::MoveAnchor);
    tempcursor.setPosition(pos,QTextCursor::KeepAnchor);
    editor->setTextCursor(tempcursor);
    tempcursor.deleteChar();
    editor->setTextCursor(oldcursor);
    cursor_locked=0;
}

void insertString(char *msg) {
    char *posstring = (char*)malloc(sizeof(char)*10);
    char *insertstring = (char*)malloc(sizeof(char)*1024);
    int pos;
    int possize = strchr(msg,'|') - msg;
    strncpy(posstring,msg,possize);
    strcpy(insertstring,msg+possize+1);
    pos=atoi(posstring);

    if(cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    cursor_locked=1;
    QTextCursor oldcursor = editor->textCursor();
    QTextCursor tempcursor = editor->textCursor();
    tempcursor.setPosition(pos,QTextCursor::MoveAnchor);
    editor->setTextCursor(tempcursor);
    editor->insertPlainText(insertstring);
    editor->setTextCursor(oldcursor);
    cursor_locked=0;
}

void executeEvent(int pos, QString string){
    if(cursor_locked){
        exit(-1); //I don't think that this thread should ever spawn race conditions, but you never know.
    }
    cursor_locked=1;
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

    //Experimental block of code concerning text highlighting
    //----------------------------------------------------
    /*QTextEdit::ExtraSelection highlight;
    highlight.cursor = editor->textCursor();
    highlight.cursor.setPosition(pos,QTextCursor::MoveAnchor);
    highlight.cursor.setPosition(pos+1,QTextCursor::KeepAnchor);
    highlight.format = editor->currentCharFormat();
    if(pos%2==0){
        highlight.format.setBackground(Qt::green);
    }else{
        highlight.format.setBackground(Qt::red);
    }
    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    editor->setExtraSelections( extras );*/
    //------------------------------------------------------

    cursor_locked=0;
}

int main(int argv, char **args){
    filename=QString("test.txt");
    QApplication app(argv,args);
    window = new MainWindow();
    Client client;
    if (argv<=1) {
        client.start("127.0.0.1", 8888);
    } else {
        client.start(args[1], 8888);
    }
    databuf=QString();
    editor = new QTextEdit();

    editor = window->editor;
    //printf("Editor: %p, window.editor: %p\n",editor,(window.editor));
    ClientEventFilter *eventFilter = new ClientEventFilter(editor);
    client.connect_signal(eventFilter,editor);

    editor->installEventFilter(eventFilter);
    window->show();
    //textEdit->show();

    return app.exec();
}
