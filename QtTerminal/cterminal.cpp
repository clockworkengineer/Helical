#include "cterminal.h"

#include <cstring>
#include <iostream>

CTerminal::CTerminal(QObject *parent) : QObject(parent)
{

    // Set character attributes/video modes.

    m_vt100FnTable["[m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[;m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[1m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[2m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[3m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[4m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[5m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[6m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[7m"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[8m"] = CTerminal::vt100Unsupported;

    m_vt100FnTable["[20l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?1l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?2l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?3l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?4l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?5l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?6l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?7l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?8l"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?9l"] = CTerminal::vt100Unsupported;

    m_vt100FnTable["[20h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?1h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?3h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?4h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?5h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?6h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?7h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?8h"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["[?9h"] = CTerminal::vt100Unsupported;

    // Cursor movement

    m_vt100FnTable["[H"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[;H"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[;r"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[A"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[B"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[C"] = CTerminal::vt100CursorMove;
    m_vt100FnTable["[D"] = CTerminal::vt100CursorMove;

    // Clear line

    m_vt100FnTable["[K"] = CTerminal::vt100ClearLine;
    m_vt100FnTable["[0K"] = CTerminal::vt100ClearLine;
    m_vt100FnTable["[1K"] = CTerminal::vt100ClearLine;
    m_vt100FnTable["[2K"] = CTerminal::vt100ClearLine;

    // Clear screen

    m_vt100FnTable["[J"] = CTerminal::vt100ClearScreen;
    m_vt100FnTable["[0J"] = CTerminal::vt100ClearScreen;
    m_vt100FnTable["[1J"] = CTerminal::vt100ClearScreen;
    m_vt100FnTable["[2J"] = CTerminal::vt100ClearScreen;

    // Vt52 compatabilty  mode

    m_vt100FnTable["="] = CTerminal::vt100Unsupported;
    m_vt100FnTable[">"] = CTerminal::vt100Unsupported;

    // Character set selection

    m_vt100FnTable["(A"] = CTerminal::vt100Unsupported;
    m_vt100FnTable[")A"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["(B"] = CTerminal::vt100Unsupported;
    m_vt100FnTable[")B"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["(0"] = CTerminal::vt100Unsupported;
    m_vt100FnTable[")0"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["(1"] = CTerminal::vt100Unsupported;
    m_vt100FnTable[")1"] = CTerminal::vt100Unsupported;
    m_vt100FnTable["(2"] = CTerminal::vt100Unsupported;
    m_vt100FnTable[")2"] = CTerminal::vt100Unsupported;

}

void CTerminal::setupTerminal(int columns, int rows)
{

    m_maxColumns = columns;
    m_maxRows = rows;

    m_terminalBuffer.reset(new std::uint8_t[rows*columns]);

    for (auto row=0; row < m_maxRows; row++) {
        std::memset(getBuffer(0, row),' ', m_maxColumns);
    }


}

void CTerminal::vt100Unsupported(CTerminal *terminal, const std::string &escapeSequence)
{
    std::cerr << "{ Esc " << escapeSequence << "} : Unsupported Escape Sequence." << std::endl;
}

void CTerminal::vt100ClearLine(CTerminal *terminal, const std::string &escapeSequence)
{
    if ((escapeSequence=="[K")||(escapeSequence=="[0K")) {
        std::memset(terminal->getBuffer(terminal->m_currentColumn, terminal->m_currentRow), ' ',
                    terminal->m_maxColumns-terminal->m_currentColumn);
    } else if (escapeSequence=="[1K") {
        std::memset(terminal->getBuffer(0, terminal->m_currentRow), ' ',
                    terminal->m_currentColumn);
    } else if (escapeSequence=="[2K") {
        std::memset(terminal->getBuffer(0, terminal->m_currentRow), ' ',
                    terminal->m_maxColumns);
    }

}

void CTerminal::vt100ClearScreen(CTerminal *terminal, const std::string &escapeSequence)
{

    if ((escapeSequence=="[J")||(escapeSequence=="[0J")) {
        vt100ClearLine(terminal, "[K");
        for (auto row=terminal->m_currentRow+1; row < terminal->m_maxRows; row++) {
            std::memset(terminal->getBuffer(0, row), ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[1J") {
        vt100ClearLine(terminal, "[1K");
        for (auto row=0; row < terminal->m_currentRow; row++) {
            std::memset(terminal->getBuffer(0, row), ' ', terminal->m_maxColumns);
        }
    } else if (escapeSequence=="[2J") {
        for (auto row=0; row < terminal->m_maxRows; row++) {
            std::memset(terminal->getBuffer(0, row),' ', terminal->m_maxColumns);
        }
    }


}

void CTerminal::vt100CursorMove(CTerminal *terminal, const std::string &escapeSequence)
{

    if((escapeSequence == "[H") || (escapeSequence == "[;H")) {
        terminal->m_currentRow = terminal->m_currentColumn = 0;
    } else if (escapeSequence.back()=='A') {
        terminal->m_currentRow -= terminal->extractNumber(escapeSequence);
    } else if (escapeSequence.back()=='B') {
        terminal->m_currentRow += terminal->extractNumber(escapeSequence);
    } else if (escapeSequence.back()=='C') {
        terminal->m_currentColumn += terminal->extractNumber(escapeSequence);
    } else if (escapeSequence.back()=='D') {
        terminal->m_currentColumn -= terminal->extractNumber(escapeSequence);
    } else {
        std::string coordinates { escapeSequence };
        coordinates = coordinates.substr(1);
        coordinates.resize(coordinates.size()-1);
        terminal->m_currentRow = std::min((std::stoi(coordinates)-1), (terminal->m_maxRows-1));
        coordinates = coordinates.substr(coordinates.find(';')+1);
        terminal->m_currentColumn = std::min((std::stoi(coordinates)-1),  (terminal->m_maxColumns-1));
    }
}

void CTerminal::processEscapeSequence(std::deque<std::uint8_t> &textToProcess)
{
    std::string escapeSeqence;
    std::string matchSeqence;

    textToProcess.pop_front();

    while (!textToProcess.empty()) {
        escapeSeqence.append(1,textToProcess.front());
        if (!std::isdigit(textToProcess.front())) {
            matchSeqence.append(1,textToProcess.front());
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
        if (std::isprint(sequence)) {
            std::cout << "[" << sequence << "]";
        }else {
            std::cout << "[" << static_cast<int>(sequence) << "]";
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

void CTerminal::processCharacter(std::deque<std::uint8_t> &textToProcess)
{

    switch(textToProcess.front()) {

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
        *getBuffer(m_currentColumn, m_currentRow) = textToProcess.front();
        m_currentColumn++;
        break;

    }
}

std::uint8_t* CTerminal::getBuffer(int column, int row)
{
    return m_terminalBuffer.get()+(row*m_maxColumns+column);
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

int CTerminal::extractNumber(const std::string &numberToExtract)
{
    std::string number {numberToExtract};
    number = number.substr(1);
    number.resize(number.size()-1);
    if (!number.empty()){
        return(std::stoi(number));
    }

    return(0);

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
