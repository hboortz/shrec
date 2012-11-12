#ifndef TESTQT_H
#define TESTQT_H
 
//int add(int x, int y); // function prototype for add.h

class OnKeyPress: public QObject{
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

int receiveEvent(int pos, int event);

#endif