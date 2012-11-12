#ifndef SERVER_H
#define SERVER_H

class OnKeyPress: public QObject{
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);

#endif