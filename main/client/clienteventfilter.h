#ifndef CLIENT_EVENT_FILTER_H
#define CLIENT_EVENT_FILTER_H

#include "../common.h"

class ClientEventFilter : public QObject {
Q_OBJECT
public:
  ClientEventFilter(QTextEdit *inputEditor, int *input_cursor_locked);
  bool eventFilter(QObject *obj, QEvent *event);
private:
  QTextEdit *editor;
  int *cursor_locked;

signals:
  void signalWrite(Action action, char *msg);
  void cut();
  void paste();
};
#endif