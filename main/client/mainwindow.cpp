/****************************************************************************
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 ** Framework used and modified by Heather Boortz and Derek Redfern
 ****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include <string>


MainWindow::MainWindow()
{
    editor = new QTextEdit;


    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(editor);
    widget->setLayout(layout);



    createActions();
    createMenus();
    setupEditor();

    QPalette* pal = new QPalette();
    pal->setColor(QPalette::Base,QColor(20,20,20));
    pal->setColor(QPalette::Text,QColor(255,255,255));
    editor->setPalette(*pal);

    //QString message = tr("A context menu is available by right-clicking");
    //statusBar()->showMessage(message);

    setWindowTitle(tr("Secret-Octo-Ninja"));
    setMinimumSize(160, 160);
    resize(480, 320);
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    //editor = new QTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    //QFile file("mainwindow.h");
    //if (file.open(QFile::ReadOnly | QFile::Text))
    //    editor->setPlainText(file.readAll());
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}


void MainWindow::cut()
{
    QTextCursor cursor = editor->textCursor();

    int position = cursor.position();
    int anchor = cursor.anchor();
    
    QClipboard * clipboard = QApplication::clipboard();

    if(position!=anchor) {
        clipboard->setText(cursor.selectedText());
    }

    Action action = REMOVE_STRING;
    char *msg = (char *) malloc(sizeof(char)*20);
    sprintf(msg, "%i|%i",position,anchor);
    emit signalWrite(action, msg);

}

void MainWindow::copy()
{
    QTextCursor cursor = editor->textCursor();

    int position = cursor.position();
    int anchor = cursor.anchor();
    
    QClipboard * clipboard = QApplication::clipboard();

    if(position!=anchor) {
        clipboard->setText(cursor.selectedText());
    }
}

void MainWindow::paste()
{
    QClipboard * clipboard = QApplication::clipboard();

    QTextCursor cursor = editor->textCursor();
    int position = cursor.position();
    Action action = INSERT_STRING;
    char *msg = (char *) malloc(sizeof(char)*1024);
    sprintf(msg, "%i|%s",position,clipboard->text().toLocal8Bit().data());

    emit signalWrite(action, msg);
}

void MainWindow::createActions()
{
    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
}
