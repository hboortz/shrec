#ifndef CLIENT_EVENT_FILTER_H
#define CLIENT_EVENT_FILTER_H

#include "../common.h"

class ClientEventFilter : public QObject {
Q_OBJECT
public:
  ClientEventFilter(QTextEdit *editor);
  bool eventFilter(QObject *obj, QEvent *event);
private:
  QTextEdit *editor;
signals:
  void signalWrite(Action action, char *msg);
};
#endif