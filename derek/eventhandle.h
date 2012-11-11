#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
MainWindow();
protected:
bool eventFilter(QObject *obj, QEvent *ev);
private:
QTextEdit *textEdit;
};