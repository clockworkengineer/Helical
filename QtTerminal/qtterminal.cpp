#include "qtterminal.h"

#include <QDebug>
#include <QScrollBar>
#include <cstring>
#include <iostream>

QtTerminal::QtTerminal(int columns, int rows, QWidget *parent) : QListView(parent)
{

    m_terminal.initializeTerminal(columns, rows);
    m_terminal.setScreenScroll(scrollScreenUp, this);

    setStyleSheet("QListView {selection-background-color: white; selection-color: white;}");

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    setFont(font);
    QFontMetrics fm(property("font").value<QFont>());

    int extraCols=verticalScrollBar()->height()/fm.maxWidth();
    int extraRows=horizontalScrollBar()->height()/fm.height()-1;

    setFixedWidth(fm.maxWidth()*(m_terminal.getMaxColumns()+extraCols));
    setFixedHeight(fm.height()*(m_terminal.getMaxRows()+extraRows));

    setUniformItemSizes(true);

    horizontalScrollBar()->setDisabled(true);
    horizontalScrollBar()->setHidden(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setSelectionRectVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);
    setSelectionMode(QListView::NoSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setSelectionBehavior(QListView::SelectItems);

    setModel(&m_terminalModel);

    m_terminalModel.insertRows(0, m_terminal.getMaxRows());

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

void QtTerminal::terminalOutput(const QString &characters)
{
    std::deque<std::uint8_t> charactersToProcess;

    for (auto byte : characters) {
        charactersToProcess.push_back(byte.toLatin1());
    }

    while(!charactersToProcess.empty()) {
        m_terminal.processCharacter(charactersToProcess);
        if(!charactersToProcess.empty())charactersToProcess.pop_front();
    }

    bufferToScreen();

}
