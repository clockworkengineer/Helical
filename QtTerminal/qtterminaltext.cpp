#include "qtterminaltext.h"

#include <QDebug>
#include <QListView>
#include <QTextCursor>

#include <cstring>
#include <iostream>


QtTerminalText::QtTerminalText(QWidget *parent) : QListView(parent)

{

    for (auto line=0; line < m_maxRows; line++) {
        std::memset(&m_terminalBuffer[line],' ', m_maxColumns);
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
    m_vt100FnTable.insert("[;m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[1m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[2m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[3m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[4m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[5m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[6m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[7m",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[8m",QtTerminalText::vt100Unsupported);

    m_vt100FnTable.insert("[20l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?1l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?2l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?3l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?4l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?5l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?6l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?7l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?8l",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("[?9l",QtTerminalText::vt100Unsupported);

    // Cursor movement

    m_vt100FnTable.insert("[H",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[;H",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[;r",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[A",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[B",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[C",QtTerminalText::vt100CursorMove);
    m_vt100FnTable.insert("[D",QtTerminalText::vt100CursorMove);

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
    m_vt100FnTable.insert(">",QtTerminalText::vt100Unsupported);

    // Character set selection

    m_vt100FnTable.insert("(A",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert(")A",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("(B",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert(")B",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("(0",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert(")0",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("(1",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert(")1",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert("(2",QtTerminalText::vt100Unsupported);
    m_vt100FnTable.insert(")2",QtTerminalText::vt100Unsupported);

}

void QtTerminalText::setupTerminal()
{

    setModel(&m_terminalModel);

    int currentRow = 0;

    m_terminalModel.insertRows(0, m_maxRows);

    for (auto line : m_terminalBuffer) {
        QString screenLine;
        for (auto cnt01=0; cnt01 < m_maxColumns; cnt01++) {
            screenLine.append(QChar(line[cnt01]));
        }
        m_terminalModel.setData(m_terminalModel.index(currentRow++), screenLine);
    }

    setSelectionRectVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);

    setFocus();

}

void QtTerminalText::processEscapeSequence(std::deque<QChar> &textToProcess)
{
    QString escapeSeqence;
    QString matchSeqence;

    textToProcess.pop_front();

    while (!textToProcess.empty()) {
        escapeSeqence.append(textToProcess.front());
        if (!textToProcess.front().isNumber()) {
            matchSeqence.append(textToProcess.front());
        }
        if (m_vt100FnTable.find(escapeSeqence)!=m_vt100FnTable.cend()) {
            m_vt100FnTable[escapeSeqence](this, escapeSeqence);
            return;
        }
        if (m_vt100FnTable.find(matchSeqence)!=m_vt100FnTable.cend()) {
            m_vt100FnTable[matchSeqence](this, escapeSeqence);
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

    m_terminalModel.insertRows(m_terminalModel.rowCount(), numberofLines);
    m_currentViewOffset+=numberofLines;
    QString screenLine;
    for (auto cnt01=0; cnt01 < m_maxColumns; cnt01++) {
        screenLine.append(QChar(m_terminalBuffer[0][cnt01]));
    }
    m_terminalModel.setData(m_terminalModel.index(m_currentViewOffset-1), screenLine);

    while (numberofLines--) {
        for (auto line=0; line < m_maxRows-1; line++) {
            std::memmove(&m_terminalBuffer[line],&m_terminalBuffer[line+1], m_maxColumns);
        }
        std::memset(&m_terminalBuffer[m_maxRows-1],' ', m_maxColumns);
    }

    setCurrentIndex(m_terminalModel.index(m_currentViewOffset+m_currentRow,m_currentColumn));

}

void QtTerminalText::processCharacter(std::deque<QChar> &textToProcess)
{


    switch(textToProcess.front().toLatin1()) {

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
        m_terminalBuffer[m_currentRow][m_currentColumn] = textToProcess.front().toLatin1();
        m_currentColumn++;
        break;

    }
}

void QtTerminalText::bufferToScreen()
{
    int currentRow=m_currentViewOffset;
    for (auto line : m_terminalBuffer) {
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
        processCharacter(textToProcess);
        if(!textToProcess.empty())textToProcess.pop_front();
    }

    bufferToScreen();

}

void QtTerminalText::vt100Unsupported(QtTerminalText *terminal, const QString &escapeSequence)
{
    //std::cerr << "Esc " << escapeSequence.toStdString() << " : Unsupported Escape Sequence." << std::endl;
}

void QtTerminalText::vt100ClearLine(QtTerminalText *terminal, const QString &escapeSequence)
{
    if ((escapeSequence=="[K")||(escapeSequence=="[0K")) {
        std::memset(&terminal->m_terminalBuffer[terminal->m_currentRow][terminal->m_currentColumn], ' ',
                       terminal->m_maxColumns-terminal->m_currentColumn);
    } else if (escapeSequence=="[1K") {
        std::memset(&terminal->m_terminalBuffer[terminal->m_currentRow][0], ' ',
                       terminal->m_currentColumn);
    } else if (escapeSequence=="[2K") {
        std::memset(&terminal->m_terminalBuffer[terminal->m_currentRow][0], ' ',
                       terminal->m_maxColumns);
    }

}

void QtTerminalText::vt100ClearScreen(QtTerminalText *terminal, const QString &escapeSequence)
{

    if ((escapeSequence=="[J")||(escapeSequence=="[0J")) {
        vt100ClearLine(terminal, "[K");
        for (auto line=terminal->m_currentRow+1; line < terminal->m_maxRows; line++) {
            std::memset(&terminal->m_terminalBuffer[line][0], ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[1J") {
        vt100ClearLine(terminal, "[1K");
        for (auto line=0; line < terminal->m_currentRow; line++) {
            std::memset(&terminal->m_terminalBuffer[line][0], ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[2J") {
        for (auto line=0; line < terminal->m_maxRows; line++) {
            std::memset(&terminal->m_terminalBuffer[line],' ', terminal->m_maxColumns);
        }
    }


}

void QtTerminalText::vt100CursorMove(QtTerminalText *terminal, const QString &escapeSequence)
{

    if((escapeSequence == "[H") || (escapeSequence == "[;H")) {
        terminal->m_currentRow = terminal->m_currentColumn = 0;
    } else if (escapeSequence.endsWith('A')) {
        QString number {escapeSequence};
        number = number.remove(0,1);
        number.resize(number.size()-1);
        terminal->m_currentRow -= number.toInt();
    } else if (escapeSequence.endsWith('B')) {
        QString number {escapeSequence};
        number = number.remove(0,1);
        number.resize(number.size()-1);
        terminal->m_currentRow += number.toInt();
    } else if (escapeSequence.endsWith('C')) {
        QString number {escapeSequence};
        number = number.remove(0,1);
        number.resize(number.size()-1);
        terminal->m_currentColumn += number.toInt();
    } else if (escapeSequence.endsWith('D')) {
        QString number {escapeSequence};
        number = number.remove(0,1);
        number.resize(number.size()-1);
        terminal->m_currentColumn -= number.toInt();
    } else {
        QString coordinates { escapeSequence};
        QStringList numbers;
        coordinates = coordinates.remove(0,1);
        coordinates.resize(coordinates.size()-1);
        numbers = coordinates.split(";");
        if (numbers.size()==2) {
            terminal->m_currentRow = std::min((numbers[0].toInt()-1), (terminal->m_maxRows-1));
            terminal->m_currentColumn = std::min((numbers[1].toInt()-1),  (terminal->m_maxColumns-1));
        }
    }

}

