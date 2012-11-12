#ifndef CLIENT_H
#define CLIENT_H

class OnKeyPress: public QObject{
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

int receiveEvent(int pos, int event);
int sendEvent(int pos, int event);
void saveData();

#endif