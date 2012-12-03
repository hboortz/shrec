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

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void newFile();
    void open();
    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void about();
    void aboutQt();

private:
    void createActions();
    void createMenus();
    void setupEditor();
    QTextEdit *editor;
    Highlighter *highlighter;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    //QActionGroup *alignmentGroup;
    QAction *newAct;
    QAction *openAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLabel *infoLabel;
};

#endif
