#include "server.h"
using namespace std;

static QString databuf;
QTextEdit *textEdit;
QString filename;
QList<QTcpSocket*> clients;

//----------------------------------------------------------------

Server::Server(QObject* parent): QObject(parent)
{
    connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

    server.listen(QHostAddress::Any, 8888);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(saveData()));
    timer->start(15000);
}

Server::~Server()
{
    server.close();
}

void Server::acceptConnection()
{
    clients.append(server.nextPendingConnection());
    printf("connected to ip %s\n",clients.last()->localAddress().toString().toLocal8Bit().data());
    connect(clients.last(), SIGNAL(readyRead()), this, SLOT(startRead()));
    connect(clients.last(), SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    initialWrite(clients.last());

    //add the cursor to other clients' screens
    char *newmsg = (char*)malloc(sizeof(char)*100);
    Action action = CURSOR_MOVE;
    sprintf(newmsg,"%s|%i|%i",clients.last()->localAddress().toString().toLocal8Bit().data(),0,0);
    addMetadata(action,newmsg);
    broadcastAction(newmsg,clients.last());
    free(newmsg);
}

void Server::clientDisconnect() {
    int i;

    //Remove the client from the local list
    for(i=0;i<clients.size();i++) {
        if(clients.at(i)==sender()) {
            //Clear the cursor from the other clients' screens
            char *newmsg = (char*)malloc(sizeof(char)*100);
            Action action = CURSOR_MOVE;
            sprintf(newmsg,"%s|%i|%i",clients.at(i)->localAddress().toString().toLocal8Bit().data(),-1,-1);
            addMetadata(action,newmsg);
            broadcastAction(newmsg,clients.at(i));
            free(newmsg);

            clients.removeAt(i);
            puts("client disconnected");
        }
    }
}

void Server::initialWrite(QTcpSocket *client)
{
    //char *text = textEdit->toPlainText().toLocal8Bit().data(); ///Is something going wrong here?
    int length = strlen(textEdit->toPlainText().toLocal8Bit().data());
    //@printf("This should be length: %i\n",length);
    char *text = (char*)malloc(sizeof(char)*(length+1));
    char *temp = (char*)malloc(sizeof(char)*1024);
    strcpy(text,textEdit->toPlainText().toLocal8Bit().data());
    //@printf("Copied length: %i",static_cast<int>(strlen(temp)));
    Action action = INITIAL_SEND;
    while(length>0){
        //@printf("Before truncating: %s\n",text);

        strncpy(temp,text,1015);
        temp[1015] = '\0';
        //@printf("Part of the initial write: %s\n",temp);
        addMetadata(action,temp);
        //@printf("Part of the initial write: %s\n",temp);
        client->write(temp);
        length-=1015;
        text+=1015;
    }
}

void Server::startRead()
{
    QTcpSocket *readClient = qobject_cast<QTcpSocket *>(sender());
    char *buffer = (char*)malloc(sizeof(char)*10240);
    int bytesAvail = readClient->bytesAvailable();
    readClient->read(buffer, bytesAvail);
    buffer[bytesAvail] = '\0';
    //@printf("%s\n",buffer);
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
            case KEY_EVENT: {
                receiveEvent(stringToEvent(msg));
                addMetadata(*action,msg);
                broadcastAction(msg,NULL);
                }
                break;
            case INSERT_STRING:{
                insertString(msg);
                addMetadata(*action,msg);
                broadcastAction(msg,NULL);
                }
                break;
            case REMOVE_STRING: {
                removeString(msg);
                addMetadata(*action,msg);
                broadcastAction(msg,NULL);
                }
                break;
            case INITIAL_SEND: {
                //@puts("That's weird. Clients should never send that action.");
                exit(-1);
                }
                break;
            case CURSOR_MOVE: {
                //cursorMove(msg);
                char *newmsg = (char*)malloc(sizeof(char)*100);
                sprintf(newmsg,"%s|%s",readClient->localAddress().toString().toLocal8Bit().data(),msg);
                addMetadata(*action,newmsg);
                broadcastAction(newmsg,readClient);
                free(newmsg);
                }
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

void Server::broadcastAction(char *string, QTcpSocket *except)
{
    int i;
    //printf("%s\n",string);
    //char *string = eventToString(event);
    for(i=0;i<clients.size();i++){
        if(clients[i]!=except) {
            clients[i]->write(string);
        }
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
    } else if (event.nvk==65289) { //tab
        text = QString("    ");
    } else {
        text = QString("");
    }
    //broadcastAction(eventToString(event));
    executeEvent(event.pos,text);
    return(0);
}

void Server::saveData(){
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

void insertString(char *msg) {
    char *posstring = (char*)malloc(sizeof(char)*10);
    char *insertstring = (char*)malloc(sizeof(char)*1024);
    int pos;
    int possize = strchr(msg,'|') - msg;
    strncpy(posstring,msg,possize);
    strcpy(insertstring,msg+possize+1);
    pos=atoi(posstring);

    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor tempcursor = textEdit->textCursor();
    tempcursor.setPosition(pos,QTextCursor::MoveAnchor);
    textEdit->setTextCursor(tempcursor);
    textEdit->insertPlainText(insertstring);
    textEdit->setTextCursor(oldcursor);
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

    QTextCursor oldcursor = textEdit->textCursor();
    QTextCursor tempcursor = textEdit->textCursor();
    tempcursor.setPosition(anchor,QTextCursor::MoveAnchor);
    tempcursor.setPosition(pos,QTextCursor::KeepAnchor);
    textEdit->setTextCursor(tempcursor);
    tempcursor.deleteChar();
    textEdit->setTextCursor(oldcursor);
}

int main(int argv, char **args){
    QApplication app(argv,args);
    filename=QString("test.txt");
    databuf=QString();
    textEdit = new QTextEdit();
    textEdit->setEnabled(false);
    //textEdit->setPlainText("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257 258 259 260 261 262 263 264 265 266 267 268 269 270 271 272 273 274 275 276 277 278 279 280 281 282 283 284 285 286 287 288 289 290 291 292 293 294 295 296 297 298 299 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321 322 323 324 325 326 327 328 329 330 331 332 333 334 335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 357 358 359 360 361 362 363 364 365 366 367 368 369 370 371 372 373 374 375 376 377 378 379 380 381 382 383 384 385 386 387 388 389 390 391 392 393 394 395 396 397 398 399 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 418 419 420 421 422 423 424 425 426 427 428 429 430 431 432 433 434 435 436 437 438 439 440 441 442 443 444 445 446 447 448 449 450 451 452 453 454 455 456 457 458 459 460 461 462 463 464 465 466 467 468 469 470 471 472 473 474 475 476 477 478 479 480 481 482 483 484 485 486 487 488 489 490 491 492 493 494 495 496 497 498 499 ");
    Server server;
    textEdit->show();

    return app.exec();
}