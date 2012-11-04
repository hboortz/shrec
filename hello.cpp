#include <QApplication>
#include <QLabel>
#include <QTextEdit>

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    //QLabel * label = new QLabel("<h1> <font color=green> Hello World!</font></h1>");

    //label->show();
    QTextEdit textEdit;
    textEdit.show();
    return app.exec();
}

