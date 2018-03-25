/*
 * File:   cterminal.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef CTERMINAL_H
#define CTERMINAL_H

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

#include <memory>
#include <unordered_map>
#include <functional>
#include <deque>
#include <utility>
#include <cstring>
#include <iostream>

// =================
// CLASS DECLARATION
// =================

class CTerminal
{

public:

    // Screen scroll function calback

    typedef std::function<void(void*, int)> ScreenScrollFn;

    // Constructor

    explicit CTerminal();

    // Iniitilize terminal

    void initializeTerminal(int columns, int rows);

    // Set scroll screen callback

    void setScreenScroll(ScreenScrollFn screenScrollFn, void *screenCcrollContext);

    // Process any escape sequence recieved

    void processEscapeSequence(std::deque<std::uint8_t> &sequenceToProcess);

    // Send characters to terminal to be displayed/processed.

    void processCharacter(std::deque<std::uint8_t> &charactersToProcess);

    // Scroll local screen buffer

    void scrollUp(int startRow, int endRow);

    // Call screen sroll callback

    void scrollScreenlUp(int numberofLines);

    // Get address in buffer of specific characfer location

    std::uint8_t* getBuffer(int column, int row);

    // Get various terminal attributes

    int getMaxRows() const;
    int getMaxColumns() const;
    int getCurrentRow() const;
    int getCurrentColumn() const;

private:

    // Extract number/coordinate pair from escape sequence,

    int extractNumber(const std::string &escapeSequence);
    std::pair<int,int> extractCoordinates(const std::string &escapeSequence);

    // Escape sequence to processing function mapping table

    typedef std::function<void(CTerminal *, const std::string &escapeSequence)> TerminalFn;

    static void vt100Unsupported(CTerminal *terminal,const std::string &escapeSequence);
    static void vt100ClearLine(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100ClearScreen(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100CursorMovement(CTerminal *terminal, const std::string &escapeSequence);

    static std::unordered_map<std::string, TerminalFn>  m_vt100FnTable;

    // Pointer to terminal screen buffer

    std::unique_ptr<std::uint8_t> m_terminalBuffer;

    // Terminal screen dimensions/current cursor postion

    int m_currentRow {0};
    int m_currentColumn {0};
    int m_maxRows {0};
    int m_maxColumns  {0};

    ScreenScrollFn m_screenScroll {nullptr};    // Pointer to screen scroll callback
    void *m_screeSncrollContext {nullptr};      // Scroll scroll callback context

};

#endif // CTERMINAL_H
