#include <QtGui>
#include <iostream>

class myEventFilter: public QObject
{
  public:
  myEventFilter():QObject()
  {};
  ~myEventFilter(){};

  bool eventFilter(QObject* object,QEvent* event)
  {
      if(event->type() == QEvent::KeyPress)
      {
    std::cout <<"Filtered KeyPress Event.."<<std::endl;
    return true;
      }
      else 
      {
    return QObject::eventFilter(object,event);
      }
  };  
};


int main(int argc, char **argv)
{
  QApplication app(argc,argv);
    
    QMainWindow main;
    main.show();
    app.installEventFilter(new myEventFilter());
      
    return app.exec();
}