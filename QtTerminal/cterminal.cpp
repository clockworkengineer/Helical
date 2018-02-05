#include "cterminal.h"

#include <cstring>
#include <iostream>

CTerminal::CTerminal(QObject *parent) : QObject(parent)
{

    m_vt100FnTable.insert("[20h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?1h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?3h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?4h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?5h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?6h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?7h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?8h",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?9h",CTerminal::vt100Unsupported);

    // Set character attributes/video modes.

    m_vt100FnTable.insert("[m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[;m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[1m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[2m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[3m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[4m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[5m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[6m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[7m",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[8m",CTerminal::vt100Unsupported);

    m_vt100FnTable.insert("[20l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?1l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?2l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?3l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?4l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?5l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?6l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?7l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?8l",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("[?9l",CTerminal::vt100Unsupported);

    // Cursor movement

    m_vt100FnTable.insert("[H",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[;H",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[;r",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[A",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[B",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[C",CTerminal::vt100CursorMove);
    m_vt100FnTable.insert("[D",CTerminal::vt100CursorMove);

    // Clear line

    m_vt100FnTable.insert("[K",CTerminal::vt100ClearLine);
    m_vt100FnTable.insert("[0K",CTerminal::vt100ClearLine);
    m_vt100FnTable.insert("[1K",CTerminal::vt100ClearLine);
    m_vt100FnTable.insert("[2K",CTerminal::vt100ClearLine);

    // Clear screen

    m_vt100FnTable.insert("[J",CTerminal::vt100ClearScreen);
    m_vt100FnTable.insert("[0J",CTerminal::vt100ClearScreen);
    m_vt100FnTable.insert("[1J",CTerminal::vt100ClearScreen);
    m_vt100FnTable.insert("[2J",CTerminal::vt100ClearScreen);

    // Vt52 compatabilty  mode

    m_vt100FnTable.insert("=",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(">",CTerminal::vt100Unsupported);

    // Character set selection

    m_vt100FnTable.insert("(A",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(")A",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("(B",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(")B",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("(0",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(")0",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("(1",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(")1",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert("(2",CTerminal::vt100Unsupported);
    m_vt100FnTable.insert(")2",CTerminal::vt100Unsupported);
}

void CTerminal::setupTerminal(int columns, int rows)
{

    m_maxRows = rows;
    m_maxColumns = columns;

    m_terminalBuffer = new (std::uint8_t[rows*columns]);

    for (auto row=0; row < m_maxRows; row++) {
        std::memset(getBuffer(row,0),' ', m_maxColumns);
    }


}

void CTerminal::vt100Unsupported(CTerminal *terminal, const QString &escapeSequence)
{
    //std::cerr << "Esc " << escapeSequence.toStdString() << " : Unsupported Escape Sequence." << std::endl;
}

void CTerminal::vt100ClearLine(CTerminal *terminal, const QString &escapeSequence)
{
    if ((escapeSequence=="[K")||(escapeSequence=="[0K")) {
        std::memset(terminal->getBuffer(terminal->m_currentRow, terminal->m_currentColumn), ' ',
                       terminal->m_maxColumns-terminal->m_currentColumn);
    } else if (escapeSequence=="[1K") {
        std::memset(terminal->getBuffer(terminal->m_currentRow, 0), ' ',
                       terminal->m_currentColumn);
    } else if (escapeSequence=="[2K") {
        std::memset(terminal->getBuffer(terminal->m_currentRow, 0), ' ',
                       terminal->m_maxColumns);
    }

}

void CTerminal::vt100ClearScreen(CTerminal *terminal, const QString &escapeSequence)
{

    if ((escapeSequence=="[J")||(escapeSequence=="[0J")) {
        vt100ClearLine(terminal, "[K");
        for (auto row=terminal->m_currentRow+1; row < terminal->m_maxRows; row++) {
            std::memset(terminal->getBuffer(row, 0), ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[1J") {
        vt100ClearLine(terminal, "[1K");
        for (auto row=0; row < terminal->m_currentRow; row++) {
            std::memset(terminal->getBuffer(row, 0), ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[2J") {
        for (auto row=0; row < terminal->m_maxRows; row++) {
            std::memset(terminal->getBuffer(row, 0),' ', terminal->m_maxColumns);
        }
    }


}

void CTerminal::vt100CursorMove(CTerminal *terminal, const QString &escapeSequence)
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



void CTerminal::processEscapeSequence(std::deque<QChar> &textToProcess)
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

void CTerminal::scrollScreenlUp(int numberofLines)
{
    if (m_screenScroll) {
        m_screenScroll(m_screeSncrollContext,numberofLines);
    }
}

void CTerminal::processCharacter(std::deque<QChar> &textToProcess)
{

    switch(textToProcess.front().toLatin1()) {

    case 0x1B:
        processEscapeSequence(textToProcess);
        break;

    case 0x0A:
        m_currentRow++;
        if (m_currentRow==m_maxRows) {
            m_currentRow -=1;
            scrollScreenlUp(1);
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
                scrollScreenlUp(1);
            }
        }
        *getBuffer(m_currentRow, m_currentColumn) = textToProcess.front().toLatin1();
        m_currentColumn++;
        break;

    }
}

int CTerminal::getCurrentColumn() const
{
    return m_currentColumn;
}

void CTerminal::setScreenScroll(ScreenScrollFn screenScrollFn, void *screenScrollContext)
{
    m_screenScroll = screenScrollFn;
    m_screeSncrollContext = screenScrollContext;
}

int CTerminal::getCurrentRow() const
{
    return m_currentRow;
}

int CTerminal::getMaxColumns() const
{
    return m_maxColumns;
}

int CTerminal::getMaxRows() const
{
    return m_maxRows;
}
