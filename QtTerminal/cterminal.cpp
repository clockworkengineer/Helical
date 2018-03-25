/*
 * File:   cterminal.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: CTerminal
//
// Description: Class to provide vt100 terminal. It utilizes a local buffer to simualate the
// terminal screen which may be copied to the host screen at dictated times. The emualation only
// supports cursor movement, screen clearing and basic character output with no attributes. This
// may change in he future.
//

// =============
// INCLUDE FILES
// =============

#include "cterminal.h"

//
// Escape sequence to processing function map.
//

std::unordered_map<std::string, CTerminal::TerminalFn>  CTerminal::m_vt100FnTable;

/**
 * @brief CTerminal::CTerminal
 *
 * Setup terminal.
 *
 */
CTerminal::CTerminal()
{

    if (m_vt100FnTable.empty()) {


        // Set terminal modes.

        m_vt100FnTable["[h"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["[?h"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["[l"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["[?l"] = CTerminal::vt100Unsupported;

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

        // Set single shifts

        m_vt100FnTable["N"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["O"] = CTerminal::vt100Unsupported;

        // Set character attributes/video modes.

        m_vt100FnTable["[m"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["[;m"] = CTerminal::vt100Unsupported;

        // Cursor movement

        m_vt100FnTable["[H"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[;H"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[;r"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[A"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[B"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[C"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[D"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[f"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["[;f"] = CTerminal::vt100CursorMovement;
        m_vt100FnTable["D"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["M"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["E"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["7"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["8"] = CTerminal::vt100Unsupported;

        // Tab support

        m_vt100FnTable["H"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["[g"] = CTerminal::vt100Unsupported;

        // Double height/width character support

        m_vt100FnTable["#3"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["#4"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["#5"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["#6"] = CTerminal::vt100Unsupported;

        // Clear line

        m_vt100FnTable["[K"] = CTerminal::vt100ClearLine;

        // Clear screen

        m_vt100FnTable["[J"] = CTerminal::vt100ClearScreen;

        // Device status report/get cursor position

        m_vt100FnTable["5n"] = CTerminal::vt100Unsupported;
        m_vt100FnTable["6n"] = CTerminal::vt100Unsupported;

        // LED support

        m_vt100FnTable["[q"] = CTerminal::vt100Unsupported;

    }

}

/**
 * @brief CTerminal::initializeTerminal
 *
 * Initialse terminal buffer screen.
 *
 * @param columns
 * @param rows
 */
void CTerminal::initializeTerminal(int columns, int rows)
{

    m_maxColumns = columns;
    m_maxRows = rows;

    m_terminalBuffer.reset(new std::uint8_t[rows*columns]);
    std::memset(m_terminalBuffer.get(),' ', rows*columns);

}

// ===================================
// ESCAPE SEQUENCE TRANSLATION METHODS
// ===================================

/**
 * @brief CTerminal::vt100Unsupported
 *
 * Signal not supported.
 *
 * @param terminal
 * @param escapeSequence
 */
void CTerminal::vt100Unsupported(CTerminal *terminal, const std::string &escapeSequence)
{

    (void)terminal;
    (void)escapeSequence;

    //  std::cerr << "{ Esc " << escapeSequence << "} : Unsupported Escape Sequence." << std::endl;

}

/**
 * @brief CTerminal::vt100ClearLine
 *
 * Clear line.
 *
 * @param terminal
 * @param escapeSequence
 */
void CTerminal::vt100ClearLine(CTerminal *terminal, const std::string &escapeSequence)
{
    int lengthToClear {0};
    std::uint8_t* line = terminal->getBuffer(terminal->m_currentColumn, terminal->m_currentRow);

    if ((escapeSequence=="[K")||(escapeSequence=="[0K")) {
        lengthToClear = terminal->m_maxColumns-terminal->m_currentColumn;
    } else if (escapeSequence=="[1K") {
        lengthToClear = terminal->m_currentColumn;
    } else if (escapeSequence=="[2K") {
        lengthToClear = terminal->m_maxColumns;
    }

    std::memset(line, ' ', lengthToClear);

}

/**
 * @brief CTerminal::vt100ClearScreen
 *
 * Clear a area of screen.
 *
 * @param terminal
 * @param escapeSequence
 */
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

/**
 * @brief CTerminal::vt100CursorMovement
 *
 * Various cursor movement functions.
 *
 * @param terminal
 * @param escapeSequence
 */
void CTerminal::vt100CursorMovement(CTerminal *terminal, const std::string &escapeSequence)
{

    switch(escapeSequence.back()) {
    case 'A':
        terminal->m_currentRow -= terminal->extractNumber(escapeSequence);
        break;
    case 'B':
        terminal->m_currentRow += terminal->extractNumber(escapeSequence);
        break;
    case 'C':
        terminal->m_currentColumn += terminal->extractNumber(escapeSequence);
        break;
    case 'D':
        terminal->m_currentColumn -= terminal->extractNumber(escapeSequence);
        break;
    case 'H':
    case 'f':
        std::pair<int,int> coords;
        coords=terminal->extractCoordinates(escapeSequence);
        terminal->m_currentRow = std::min((coords.first-1), (terminal->m_maxRows-1));
        terminal->m_currentColumn = std::min((coords.second-1), (terminal->m_maxColumns-1));
        break;
    }

}

/**
 * @brief CTerminal::processEscapeSequence
 *
 * Map onto correct escape sequence processing. First it tries to map the full
 * sequence then a partial sequence that has had its numeric parameters removed.
 *
 * @param escapeSequence
 */
void CTerminal::processEscapeSequence(std::deque<std::uint8_t> &sequenceToProcess)
{
    std::string fullSeqence;
    std::string partialSeqence;

    sequenceToProcess.pop_front();

    while (!sequenceToProcess.empty()) {
        fullSeqence.append(1,sequenceToProcess.front());
        if (!std::isdigit(sequenceToProcess.front())) {
            partialSeqence.append(1,sequenceToProcess.front());
        }
        if (m_vt100FnTable.find(fullSeqence)!=m_vt100FnTable.cend()) {
            m_vt100FnTable[fullSeqence](this, fullSeqence);
            return;
        }
        if (m_vt100FnTable.find(partialSeqence)!=m_vt100FnTable.cend()) {
            m_vt100FnTable[partialSeqence](this, fullSeqence);
            return;
        }
        sequenceToProcess.pop_front();
    }

    std::cout << "Esc {";
    for (auto sequence : fullSeqence) {
        std::cout << " " << (std::isprint(sequence) ? sequence : static_cast<int>(sequence));
    }
    std::cout << " }" << std::endl;

}

/**
 * @brief CTerminal::scrollScreenlUp
 *
 * Scroll buffer up a number of lines.
 *
 * @param numberofLines
 */
void CTerminal::scrollScreenlUp(int numberofLines)
{
    if (m_screenScroll) {
        m_screenScroll(m_screeSncrollContext,numberofLines);
    }
}

/**
 * @brief CTerminal::processCharacter
 *
 * Process a sequence of characters sent to terminal.
 *
 * @param charactersToProcess
 */
void CTerminal::processCharacter(std::deque<std::uint8_t> &charactersToProcess)
{

    switch(charactersToProcess.front()) {

    case 0x1B: // ESC
        processEscapeSequence(charactersToProcess);
        break;

    case 0x0A: // LINEFEED
        m_currentRow++;
        if (m_currentRow==m_maxRows) {
            m_currentRow -=1;
            scrollScreenlUp(1);
        }
        break;

    case 0x0D: // CARRIAGE RETURN
        m_currentColumn=0;
        break;

    case 0x08: // BACKSPACE
        m_currentColumn=std::max(0, m_currentColumn-1);
        break;

    case 0x0F:
    case 0x07:
        break;  // Ignore

    default:    // NORMAL PRINTABLE ASCII
        if (m_currentColumn==m_maxColumns) {
            m_currentColumn=0;
            m_currentRow++;
            if (m_currentRow==m_maxRows) {
                m_currentRow -=1;
                scrollScreenlUp(1);
            }
        }
        *getBuffer(m_currentColumn, m_currentRow) = charactersToProcess.front();
        m_currentColumn++;
        break;

    }
}

/**
 * @brief CTerminal::scrollUp
 *
 * Scroll a section of screen up one line.
 *
 * @param startRow
 * @param endRow
 */
void CTerminal::scrollUp(int startRow, int endRow)
{
    for (auto row=startRow; row < endRow-1; row++) {
        std::memmove(getBuffer(0, row), getBuffer(0, row+1), m_maxColumns);
    }
    std::memset(getBuffer(0, endRow-1),' ', m_maxColumns);
}

/**
 * @brief CTerminal::getBuffer
 *
 * Return direct pointer to character position in buffer given column/row.
 *
 * @param column
 * @param row
 * @return
 */
std::uint8_t* CTerminal::getBuffer(int column, int row)
{
    return m_terminalBuffer.get()+(row*m_maxColumns+column);
}

/**
 * @brief CTerminal::setScreenScroll
 *
 * Set screen scroll function. Used to trigger any host specific scroll screen
 * functionality.
 *
 * @param screenScrollFn
 * @param screenScrollContext
 */
void CTerminal::setScreenScroll(ScreenScrollFn screenScrollFn, void *screenScrollContext)
{
    m_screenScroll = screenScrollFn;
    m_screeSncrollContext = screenScrollContext;
}

/**
 * @brief CTerminal::extractNumber
 *
 * Extract a number from an escape sequence.
 *
 * @param escapeSequence
 * @return
 */
int CTerminal::extractNumber(const std::string &escapeSequence)
{
    std::string number {escapeSequence};

    if (!number.empty()){
        number = number.substr(1);
        number.resize(number.size()-1);
        try {
            return(std::stoi(number));
        } catch (...) {
            return(0);
        }
    }

    return(0);

}

/**
 * @brief CTerminal::extractCoordinates
 *
 * Extract a set of coordinates from an escape sequence.
 *
 * @param escapeSequence
 * @return
 */
std::pair<int,int> CTerminal::extractCoordinates(const std::string &escapeSequence)
{
    std::string coordinates { escapeSequence };
    std::pair<int,int> coords {1,1};

    try {
        coordinates = coordinates.substr(1);
        coordinates.resize(coordinates.size()-1);
        if (coordinates.front()!=';') {
            coords.first=std::stoi(coordinates);
        }
        coordinates = coordinates.substr(coordinates.find(';')+1);
        coords.second=std::stoi(coordinates);
    } catch (...) {
        return (coords);
    }

    return(coords);

}

/**
 * @brief CTerminal::getCurrentColumn
 *
 * Get currrent cursor column postion.
 *
 * @return
 */
int CTerminal::getCurrentColumn() const
{
    return m_currentColumn;
}

/**
 * @brief CTerminal::getCurrentRow
 *
 * Get current cursor row position.
 *
 * @return
 */
int CTerminal::getCurrentRow() const
{
    return m_currentRow;
}

/**
 * @brief CTerminal::getMaxColumns
 *
 * Get maximum columns for terminal.
 *
 * @return
 */
int CTerminal::getMaxColumns() const
{
    return m_maxColumns;
}

/**
 * @brief CTerminal::getMaxRows
 *
 * Get maxiumum rows for terminal.
 *
 * @return
 */
int CTerminal::getMaxRows() const
{
    return m_maxRows;
}
