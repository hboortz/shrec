#ifndef TESTQT_H
#define TESTQT_H
 
//int add(int x, int y); // function prototype for add.h

class OnKeyPress{
public:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif