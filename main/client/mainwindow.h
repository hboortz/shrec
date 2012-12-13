/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** Framework used and modified by Heather Boortz and Derek Redfern
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "highlighter.h"
#include "../common.h"
#include <QClipboard>
#include <QLineEdit>
#include <QLabel>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    QTextEdit *editor;
    QLineEdit *nameFile;
    QString fileName;
    void updateFileName();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void saveAs();
    void cut();
    void copy();
    void paste();



private:
    void createActions();
    void createMenus();
    void setupEditor();

    Highlighter *highlighter;
    QMenu *fileMenu;
    QMenu *editMenu;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

signals:
    void signalWrite(Action action, char *msg);
};

#endif
