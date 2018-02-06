#ifndef CTERMINAL_H
#define CTERMINAL_H

#include <memory>
#include <unordered_map>
#include <functional>
#include <deque>

#include <QObject>

class CTerminal : public QObject
{
    Q_OBJECT

public:

    typedef std::function<void(void*, int)> ScreenScrollFn;

    explicit CTerminal(QObject *parent = nullptr);

    void setupTerminal(int columns, int rows);

    void setScreenScroll(ScreenScrollFn screenScrollFn, void *screenCcrollContext);

    int extractNumber(const std::string &numberToExtract);

    void processEscapeSequence(std::deque<std::uint8_t> &textToProcess);
    void processCharacter(std::deque<std::uint8_t> &textToProcess);
    void scrollScreenlUp(int numberofLines);

    std::uint8_t* getBuffer(int column, int row);

    int getMaxRows() const;
    int getMaxColumns() const;
    int getCurrentRow() const;
    int getCurrentColumn() const;

signals:

public slots:

private:

    typedef std::function<void(CTerminal *, const std::string &escapeSequence)> TerminalFn;

    static void vt100Unsupported(CTerminal *terminal,const std::string &escapeSequence);
    static void vt100ClearLine(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100ClearScreen(CTerminal *terminal, const std::string &escapeSequence);
    static void vt100CursorMove(CTerminal *terminal, const std::string &escapeSequence);

    std::unordered_map<std::string, TerminalFn>  m_vt100FnTable;

    std::unique_ptr<std::uint8_t>m_terminalBuffer;

    int m_currentRow {0};
    int m_currentColumn {0};
    int m_maxRows {0};
    int m_maxColumns  {0};

    ScreenScrollFn m_screenScroll {nullptr};
    void *m_screeSncrollContext {nullptr};

};

#endif // CTERMINAL_H
