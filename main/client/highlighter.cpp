/****************************************************************************
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 ** Framework used and modified by Heather Boortz and Derek Redfern
 ****************************************************************************/

 #include <QtGui>

 #include "highlighter.h"

 Highlighter::Highlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     HighlightingRule rule;

     keywordFormat.setForeground(QColor(60,60,250));
     keywordFormat.setFontWeight(QFont::Bold);
     QStringList keywordPatterns;
     keywordPatterns << "\\bclass\\b" << "\\benum\\b" << "\\bexplicit\\b"
                     << "\\bfriend\\b" << "\\binline\\b"
                     << "\\bnamespace\\b" << "\\boperator\\b"
                     << "\\bsignals\\b" << "\\bslots\\b"
                     << "\\btemplate\\b" << "\\btypename\\b"
                     << "\\bvirtual\\b" << "\\bvolatile\\b";
     foreach (const QString &pattern, keywordPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }

     structs.setForeground(Qt::magenta);
     rule.pattern = QRegExp("\\btypedef\\b|\\bunion\\b|\\bstruct\\b");
     rule.format = structs;
     highlightingRules.append(rule);

     funcType.setForeground(Qt::yellow);
     rule.pattern = QRegExp("\\bprivate\\b|\\bprotected\\b|\\bpublic\\b|\\bvoid\\b|\\bstatic\\b");
     rule.format = funcType;
     highlightingRules.append(rule);

     dataType.setForeground(Qt::cyan);
     rule.pattern = QRegExp("\\bchar\\b|\\bint\\b|\\bshort\\b|\\blong\\b|\\bfloat\\b|\\bdouble\\b");
     rule.format = dataType;
     highlightingRules.append(rule);

     dataTypeModifier.setForeground(Qt::cyan);
     rule.pattern = QRegExp("\\bconst\\b|\\bunsigned\\b|\\bsigned\\b");
     rule.format = dataTypeModifier;
     highlightingRules.append(rule);

     /*classFormat.setFontWeight(QFont::Bold);
     classFormat.setForeground(Qt::darkMagenta);
     rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
     rule.format = classFormat;
     highlightingRules.append(rule);*/

     singleLineCommentFormat.setForeground(Qt::red);
     rule.pattern = QRegExp("//[^\n]*");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     multiLineCommentFormat.setForeground(Qt::red);

     quotationFormat.setForeground(Qt::darkGreen);
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);

     functionFormat.setFontItalic(true);
     functionFormat.setForeground(QColor(60,60,250));
     rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
     rule.format = functionFormat;
     highlightingRules.append(rule);

     commentStartExpression = QRegExp("/\\*");
     commentEndExpression = QRegExp("\\*/");
 }

 void Highlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);

     while (startIndex >= 0) {
         int endIndex = commentEndExpression.indexIn(text, startIndex);
         int commentLength;
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         setFormat(startIndex, commentLength, multiLineCommentFormat);
         startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
     }
 }
