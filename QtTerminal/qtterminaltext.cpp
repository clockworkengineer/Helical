#include "qtterminaltext.h"

#include <QDebug>
#include <QListView>
#include <QTextCursor>

#include <cstring>
#include <iostream>


QtTerminalText::QtTerminalText(QWidget *parent) : QListView(parent)

{

    for (auto line=0; line < m_maxRows; line++) {
        std::memset(&m_terminalScreen[line],' ', m_maxColumns);
    }

    m_vt100FnTable.insert("[20h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?1h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?3h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?4h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?5h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?6h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?7h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?8h",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?9h",QtTerminalText::vt100Unsupported);

    // Set character attributes/video modes.

    m_vt100FnTable.insert("[m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[1m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[2m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[3m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[4m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[5m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[6m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[7m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[8m",QtTerminalText::vt100Unsupported);

    // Cursor movement

    m_vt100FnTable.insert("[H",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[;H",QtTerminalText::vt100CursorMove);

    // Clear line

    m_vt100FnTable.insert("[K",QtTerminalText::vt100ClearLine);
    m_vt100FnTable.insert("[0K",QtTerminalText::vt100ClearLine);
    m_vt100FnTable.insert("[1K",QtTerminalText::vt100ClearLine);
    m_vt100FnTable.insert("[2K",QtTerminalText::vt100ClearLine);

    // Clear screen

    m_vt100FnTable.insert("[J",QtTerminalText::vt100ClearScreen);
    m_vt100FnTable.insert("[0J",QtTerminalText::vt100ClearScreen);
    m_vt100FnTable.insert("[1J",QtTerminalText::vt100ClearScreen);
    m_vt100FnTable.insert("[2J",QtTerminalText::vt100ClearScreen);

    // Vt52 compatabilty  mode

    m_vt100FnTable.insert("=",QtTerminalText::vt100Unsupported);


}

void QtTerminalText::setupTerminal()
{

    setModel(&m_terminalModel);

    int currentRow = 0;

    m_terminalModel.insertRows(0, m_maxRows);

    for (auto line : m_terminalScreen) {
        QString screenLine;
        for (auto cnt01=0; cnt01 < m_maxColumns; cnt01++) {
            screenLine.append(QChar(line[cnt01]));
        }
        m_terminalModel.setData(m_terminalModel.index(currentRow++), screenLine);
    }

    setFocus();

}

void QtTerminalText::processEscapeSequence(std::deque<QChar> &textToProcess)
{
    QString escapeSeqence;

    textToProcess.pop_front();

    while (!textToProcess.empty()) {
        escapeSeqence.append(textToProcess.front());
        if (m_vt100FnTable.find(escapeSeqence)!=m_vt100FnTable.cend()) {
            m_vt100FnTable[escapeSeqence](this, escapeSeqence);
            return;
        }
        textToProcess.pop_front();
    }

    std::cout << "Esc ";
    for (auto sequence : escapeSeqence) {
        if (sequence.isPrint()) {
            std::cout << "[" << sequence.toLatin1() << "]";
        }else {
            std::cout << "[" << static_cast<int>(sequence.toLatin1()) << "]";
        }
    }
    std::cout << std::endl;

}

void QtTerminalText::scrollUp(int numberofLines)
{

    while (numberofLines--) {
        for (auto line=0; line < m_maxRows-1; line++) {
            std::memmove(&m_terminalScreen[line],&m_terminalScreen[line+1], m_maxColumns);
        }
        std::memset(&m_terminalScreen[m_maxRows-1],' ', m_maxColumns);
    }
}

void QtTerminalText::processNextCharacter(std::deque<QChar> &textToProcess)
{

    std::uint8_t ch = textToProcess.front().toLatin1();
    switch(ch) {
    case 0x1B:
        processEscapeSequence(textToProcess);
        break;
    case 0x0A:
        m_currentRow++;
        if (m_currentRow==m_maxRows) {
            m_currentRow -=1;
            scrollUp(1);
        }
        break;
    case 0x0D:
        m_currentColumn=0;
        break;
    case 0x08:
        m_currentColumn=std::max(0, m_currentColumn-1);
        break;
    case 0x0F:
        break;  //Ignore
    default:
        if (m_currentColumn==m_maxColumns) {
            m_currentColumn=0;
            m_currentRow++;
            if (m_currentRow==m_maxRows) {
                m_currentRow -=1;
                scrollUp(1);
            }
        }
        if (!textToProcess.front().isPrint()) {
            std::cout << static_cast<int>(textToProcess.front().toLatin1()) << std::endl;
        }
        m_terminalScreen[m_currentRow][m_currentColumn] = ch;
        m_currentColumn++;
        break;
    }
}

void QtTerminalText::textToScreen()
{
    int currentRow=0;
    for (auto line : m_terminalScreen) {
        QString screenLine;
        for (auto cnt01=0; cnt01 < m_maxColumns; cnt01++) {
            screenLine.append(QChar(line[cnt01]));
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
        processNextCharacter(textToProcess);
        if(!textToProcess.empty())textToProcess.pop_front();
    }
    textToScreen();
    setCurrentIndex(m_terminalModel.index(m_currentRow-1,m_currentColumn-1));

}

void QtTerminalText::vt100Unsupported(QtTerminalText *terminal, const QString &escapeSequence)
{
   // std::cerr << "Esc " << escapeSequence.toStdString() << " : Unsupported Escape Sequence." << std::endl;
}

void QtTerminalText::vt100ClearLine(QtTerminalText *terminal, const QString &escapeSequence)
{
    if ((escapeSequence=="[K")||(escapeSequence=="[0K")) {
        std::memset(&terminal->m_terminalScreen[terminal->m_currentRow][terminal->m_currentColumn], ' ',
                       terminal->m_maxColumns-terminal->m_currentColumn);
    } else if (escapeSequence=="[1K") {
        std::memset(&terminal->m_terminalScreen[terminal->m_currentRow][0], ' ',
                       terminal->m_currentColumn);
    } else if (escapeSequence=="[2K") {
        std::memset(&terminal->m_terminalScreen[terminal->m_currentRow][0], ' ',
                       terminal->m_maxColumns);
    }

}

void QtTerminalText::vt100ClearScreen(QtTerminalText *terminal, const QString &escapeSequence)
{

    if ((escapeSequence=="[J")||(escapeSequence=="[0J")) {
        vt100ClearLine(terminal, "[K");
        for (auto line=terminal->m_currentRow+1; line < terminal->m_maxRows; line++) {
            std::memset(&terminal->m_terminalScreen[line][0], ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[1J") {
        vt100ClearLine(terminal, "[1K");
        for (auto line=0; line < terminal->m_currentRow; line++) {
            std::memset(&terminal->m_terminalScreen[line][0], ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[2J") {
        for (auto line=0; line < terminal->m_maxRows; line++) {
            std::memset(&terminal->m_terminalScreen[line],' ', terminal->m_maxColumns);
        }
    }


}

void QtTerminalText::vt100CursorMove(QtTerminalText *terminal, const QString &escapeSequence)
{

    if((escapeSequence=="[H")||(escapeSequence=="[;H")) {
        terminal->m_currentRow = terminal->m_currentColumn = 0;
    }

}

