/****************************************************************************
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 ** Framework used and modified by Heather Boortz and Derek Redfern
  ****************************************************************************/

 #include <QApplication>
 #include <QtGui>
 #include "mainwindow.h"

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);

     QVBoxLayout *layout = new QVBoxLayout;

     MainWindow window;
     window.show();
     return app.exec();
 }
