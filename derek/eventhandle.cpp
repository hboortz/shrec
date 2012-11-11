#include <QtGui/QApplication>
#include <QtGui/QTextEdit>
#include <QtGui>
#include "eventhandle.h"

MainWindow::MainWindow()
{
    textEdit = new QTextEdit();
    setCentralWidget(textEdit);
    textEdit->installEventFilter(this);
    textEdit.show();
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == textEdit) {
    if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    qDebug() << "Ate key press" << keyEvent->key();
    return true;
    } else {
    return false;
    }
    } else {
    // pass the event on to the parent class
    return QMainWindow::eventFilter(obj, event);
    }
} 