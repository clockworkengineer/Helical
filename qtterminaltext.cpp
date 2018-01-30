#include "qtterminaltext.h"

QtTerminalText::QtTerminalText(QWidget *parent) : QPlainTextEdit(parent)

{

}

void QtTerminalText::keyPressEvent(QKeyEvent *event)
{

    switch(event->key()){

    case Qt::Key_Up:
        emit keySend("\033[A");
        break;

    case Qt::Key_Down:
        emit keySend("\033[B");
        break;

    case Qt::Key_Left:
        emit keySend("\033[D");
        break;

    case Qt::Key_Right:
        emit keySend("\033[C");
        break;

    default:
        emit keySend(event->text().toLatin1());
        break;

    }

}

void QtTerminalText::terminalOutput(const QString &text)
{

    this->insertPlainText(text);
    this->moveCursor (QTextCursor::End);

}

