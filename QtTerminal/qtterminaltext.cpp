#include "qtterminaltext.h"

#include <deque>
#include <iostream>

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
    std::deque<QChar> textToProcess {text.begin(), text.end()};
    QString escapeSeqence;
    QChar character;

    while(!textToProcess.empty()) {

        character = textToProcess.front();

        if (character.toLatin1() == 0x1b) {
            escapeSeqence.append(character);
            textToProcess.pop_front();
            while (!textToProcess.empty()) {
                character = textToProcess.front();
                escapeSeqence.append(character);
                textToProcess.pop_front();
                if (character.toLatin1() == 0x0a) {
                    break;
                }
            }

        } else if (character.toLatin1() != 0x0a) {
            if (character.isPrint() || (character.toLatin1() == 0x0d)) {
                this->insertPlainText(character);
            } else {
                std::cout << static_cast<int>(character.toLatin1()) << std::endl;
            }
            textToProcess.pop_front();

        } else {
            textToProcess.pop_front();
        }

    }

    this->moveCursor (QTextCursor::End);

}

