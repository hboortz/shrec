 /****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 ****************************************************************************/

 #ifndef HIGHLIGHTER_H
 #define HIGHLIGHTER_H

 #include <QSyntaxHighlighter>

 #include <QHash>
 #include <QTextCharFormat>
 #include <QObject>

 class QTextDocument;

 class Highlighter : public QSyntaxHighlighter
 {
     Q_OBJECT

 public:
     Highlighter(QTextDocument *parent = 0);

 protected:
     void highlightBlock(const QString &text);

 private:
     struct HighlightingRule
     {
         QRegExp pattern;
         QTextCharFormat format;
     };
     QVector<HighlightingRule> highlightingRules;

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;
     QTextCharFormat keywordFormat;
     QTextCharFormat loops;
     QTextCharFormat structs;
     QTextCharFormat funcType;
     QTextCharFormat dataType;
     QTextCharFormat dataTypeModifier;
     QTextCharFormat classFormat;
     QTextCharFormat singleLineCommentFormat;
     QTextCharFormat multiLineCommentFormat;
     QTextCharFormat quotationFormat;
     QTextCharFormat functionFormat;
 };
#endif
