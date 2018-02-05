#include "qtterminaltext.h"

#include <QDebug>
#include <QListView>
#include <QTextCursor>

#include <cstring>
#include <iostream>


QtTerminalText::QtTerminalText(QWidget *parent) : QListView(parent)

{

    m_terminal.setupTerminal(80, 24);
    m_terminal.setScreenScroll(scrollScreenUp, this);

}

void QtTerminalText::setupTerminalText()
{

    setModel(&m_terminalModel);

    int currentRow = 0;

    m_terminalModel.insertRows(0, m_terminal.getMaxRows());

    for (auto row=0; row < m_terminal.getMaxRows(); row++) {
        QString screenLine;
        for (auto column=0; column < m_terminal.getMaxColumns(); column++) {
            screenLine.append(QChar(*m_terminal.getBuffer(row, column)));
        }
        m_terminalModel.setData(m_terminalModel.index(currentRow++), screenLine);
    }

    setSelectionRectVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);

    setFocus();

}

void QtTerminalText::scrollScreenUp(void *terminalConext, int numberofLines)
{

    QtTerminalText *terminalText { static_cast<QtTerminalText *>(terminalConext) };

    terminalText->m_terminalModel.insertRows(terminalText->m_terminalModel.rowCount(), numberofLines);
    terminalText->m_currentViewOffset+=numberofLines;
    QString screenLine;
    for (auto column=0; column < terminalText->m_terminal.getMaxRows(); column++) {
        screenLine.append(QChar(*terminalText->m_terminal.getBuffer(0, column)));
    }
    terminalText->m_terminalModel.setData(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset-1), screenLine);

    while (numberofLines--) {
        for (auto row=0; row < terminalText->m_terminal.getMaxRows()-1; row++) {
            std::memmove(terminalText->m_terminal.getBuffer(row, 0), terminalText->m_terminal.getBuffer(row+1, 0),
                         terminalText->m_terminal.getMaxColumns());
        }
        std::memset(terminalText->m_terminal.getBuffer(terminalText->m_terminal.getMaxRows()-1, 0),' ',
                    terminalText->m_terminal.getMaxColumns());
    }

    terminalText->setCurrentIndex(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset+terminalText->m_terminal.getCurrentRow(),
                                                        terminalText->m_terminal.getCurrentColumn()));

}

void QtTerminalText::bufferToScreen()
{
    int currentRow=m_currentViewOffset;
    for (auto row=0; row < m_terminal.getMaxRows(); row++) {
        QString screenLine;
        for (auto column=0; column < m_terminal.getMaxColumns(); column++) {
            screenLine.append(QChar(*m_terminal.getBuffer(row, column)));
        }
        m_terminalModel.setData(m_terminalModel.index(currentRow++), screenLine);
    }

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

    while(!textToProcess.empty()) {
        m_terminal.processCharacter(textToProcess);
        if(!textToProcess.empty())textToProcess.pop_front();
    }

    bufferToScreen();

}
