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
// Class: QtTerminal
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include <memory>
#include <unordered_map>
#include <functional>
#include <deque>
#include <utility>

// =================
// CLASS DECLARATION
// =================

class CTerminal
{

public:

    typedef std::function<void(void*, int)> ScreenScrollFn;

    explicit CTerminal();

    void initializeTerminal(int columns, int rows);

    void setScreenScroll(ScreenScrollFn screenScrollFn, void *screenCcrollContext);

    int extractNumber(const std::string &escapeSequence);
    std::pair<int,int> extractCoordinates(const std::string &escapeSequence);

    void processEscapeSequence(std::deque<std::uint8_t> &sequenceToProcess);
    void processCharacter(std::deque<std::uint8_t> &charactersToProcess);
    void scrollScreenlUp(int numberofLines);

    std::uint8_t* getBuffer(int column, int row);

    int getMaxRows() const;
    int getMaxColumns() const;
    int getCurrentRow() const;
    int getCurrentColumn() const;

private:

    typedef std::function<void(CTerminal *, const std::string &escapeSequence)> TerminalFn;

    static void vt100Unsupported(CTerminal *terminal,const std::string &escapeSequence);
    static void vt100ClearLine(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100ClearScreen(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100CursorMovement(CTerminal *terminal, const std::string &escapeSequence);

    std::unordered_map<std::string, TerminalFn>  m_vt100FnTable;

    std::unique_ptr<std::uint8_t> m_terminalBuffer;

    int m_currentRow {0};
    int m_currentColumn {0};
    int m_maxRows {0};
    int m_maxColumns  {0};

    ScreenScrollFn m_screenScroll {nullptr};
    void *m_screeSncrollContext {nullptr};

};

#endif // CTERMINAL_H
