#ifndef QTTERMINALTEXT_H
#define QTTERMINALTEXT_H

#include <QObject>
#include <QListView>
#include <QStringListModel>
#include <QKeyEvent>
#include <QHash>

#include <functional>
#include <deque>


class QtTerminalText : public QListView
{
    Q_OBJECT

public:

    typedef std::function<void(QtTerminalText *, const QString &escapeSequence)> TerminalFn;

    QtTerminalText(QWidget *parent = 0);

    void setupTerminal();
    void processEscapeSequence(std::deque<QChar> &textToProcess);
    void processNextCharacter(std::deque<QChar> &textToProcess);
    void scrollUp(int numberofLines);

    void textToScreen();

protected:

    virtual void keyPressEvent(QKeyEvent *e);

signals:

    void keySend(const QByteArray &keyAscii);

public slots:

    void terminalOutput(const QString &text);

private:

    static void vt100Unsupported(QtTerminalText *terminal,const QString &escapeSequence);
    static void vt100ClearLine(QtTerminalText *terminal, const QString &escapeSequence);
    static void vt100ClearScreen(QtTerminalText *terminal, const QString &escapeSequence);
    static void vt100CursorMove(QtTerminalText *terminal, const QString &escapeSequence);

    QHash<QString, TerminalFn>  m_vt100FnTable;

    QStringListModel m_terminalModel;

    std::uint8_t m_terminalScreen[24][80];
    int m_currentRow {0};
    int m_currentColumn {0};
    const int m_maxColumns  {80};
    const int m_maxRows {24};

};

#endif // QTTERMINALTEXT_H