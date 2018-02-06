#include "qtterminal.h"

#include <cstring>
#include <iostream>

QtTerminal::QtTerminal(QWidget *parent) : QListView(parent)

{

}

void QtTerminal::setupTerminalText(int columns, int rows)
{

   int currentRow = 0;

    m_terminal.initializeTerminal(columns, rows);
    m_terminal.setScreenScroll(scrollScreenUp, this);

    setModel(&m_terminalModel);

    m_terminalModel.insertRows(0, m_terminal.getMaxRows());

    for (auto row=0; row < m_terminal.getMaxRows(); row++) {
        QString screenLine;
        for (auto column=0; column < m_terminal.getMaxColumns(); column++) {
            screenLine.append(QChar(*m_terminal.getBuffer(column, row)));
        }
        m_terminalModel.setData(m_terminalModel.index(currentRow++), screenLine, Qt::DisplayRole);
    }

    setSelectionRectVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);
    setSelectionMode(QListView::NoSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setSelectionBehavior(QListView::SelectItems);

    setFocus();

}

void QtTerminal::scrollScreenUp(void *terminalConext, int numberofLines)
{

    QtTerminal *terminalText { static_cast<QtTerminal *>(terminalConext) };

    terminalText->m_terminalModel.insertRows(terminalText->m_terminalModel.rowCount(), numberofLines);
    terminalText->m_currentViewOffset+=numberofLines;
    QString screenLine;
    for (auto column=0; column < terminalText->m_terminal.getMaxColumns(); column++) {
        screenLine.append(QChar(*terminalText->m_terminal.getBuffer(column, 0)));
    }
    terminalText->m_terminalModel.setData(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset-1), screenLine);

    while (numberofLines--) {
        for (auto row=0; row < terminalText->m_terminal.getMaxRows()-1; row++) {
            std::memmove(terminalText->m_terminal.getBuffer(0, row), terminalText->m_terminal.getBuffer(0, row+1),
                         terminalText->m_terminal.getMaxColumns());
        }
        std::memset(terminalText->m_terminal.getBuffer(0, terminalText->m_terminal.getMaxRows()-1),' ',
                    terminalText->m_terminal.getMaxColumns());
    }

    terminalText->setCurrentIndex(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset+terminalText->m_terminal.getCurrentRow(),
                                                        terminalText->m_terminal.getCurrentColumn()));

}

void QtTerminal::bufferToScreen()
{

    int currentRow=m_currentViewOffset;

    for (auto row=0; row < m_terminal.getMaxRows(); row++) {
        std::uint8_t*screenRow=m_terminal.getBuffer(0,row);
        std::string screenLine { &screenRow[0], &screenRow[m_terminal.getMaxColumns()]};
        m_terminalModel.setData(m_terminalModel.index(currentRow++), QString::fromStdString(screenLine), Qt::DisplayRole);
    }

}

void QtTerminal::keyPressEvent(QKeyEvent *event)
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

void QtTerminal::terminalOutput(const QString &text)
{
    std::deque<std::uint8_t> textToProcess;

    for (auto byte : text) {
        textToProcess.push_back(byte.toLatin1());
    }

    while(!textToProcess.empty()) {
        m_terminal.processCharacter(textToProcess);
        if(!textToProcess.empty())textToProcess.pop_front();
    }

    bufferToScreen();

}
