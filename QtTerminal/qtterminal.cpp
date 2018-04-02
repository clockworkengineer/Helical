/*
 * File:   qtterminal.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: QtTerminal
//
// Description: Class to act as Qt specific wrapper for class CTerminal. It is QListView that
// is used to display a QStringListModel into which the CTerminal buffer has been flushed. The
// KeyEvent function os also overridden so that key stokes may be sent down the shell channel to
// the server. A more efficient way of doing at a later date will be found; through the use
// of a custom view and model class.
//

// =============
// INCLUDE FILES
// =============
#include <QBoxLayout>
#include "qtterminal.h"
#include <QDebug>
/**
 * @brief QtTerminal::QtTerminal
 *
 * Create ListView area for terminal screen.
 *
 * @param columns
 * @param rows
 * @param parent
 */
QtTerminal::QtTerminal(int columns, int rows, QWidget *parent) : QListView(parent)
{

    m_terminal.initializeTerminal(columns, rows);
    m_terminal.setScreenScroll(scrollScreenUp, this);

    setStyleSheet("QListView {selection-background-color: white; selection-color: white;}");

    setModel(&m_terminalModel);
    m_terminalModel.insertRows(0, m_terminal.getMaxRows());

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    QFontMetrics fm(property("font").value<QFont>());

    // Code still needs work to get calculation for size of terminal window correct

    setFixedWidth(fm.width(QString(columns, 'X'))+verticalScrollBar()->sizeHint().width()+(parent->layout()->margin()/2));
    setFixedHeight((sizeHintForRow(0)*rows)+(horizontalScrollBar()->sizeHint().height()));

    horizontalScrollBar()->setDisabled(true);
    horizontalScrollBar()->setHidden(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setSelectionRectVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);
    setSelectionMode(QListView::NoSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setSelectionBehavior(QListView::SelectItems);

    setFocus();

    qDebug() << width() << verticalScrollBar()->sizeHint().width() << verticalScrollBar()->height();

}

/**
 * @brief QtTerminal::scrollScreenUp
 *
 * Scroll list view area up a number of lines offsetting the main screen buffer area
 * and saving previous content in the process.
 *
 * @param terminalConext
 * @param numberofLines
 */
void QtTerminal::scrollScreenUp(void *terminalConext, int numberofLines)
{

    QtTerminal *terminalText { static_cast<QtTerminal *>(terminalConext) };

    terminalText->m_terminalModel.insertRows(terminalText->m_terminalModel.rowCount(), numberofLines);

    while (numberofLines--) {
        std::uint8_t*screenRow=terminalText->m_terminal.getBuffer(0,0);
        std::string screenLine { &screenRow[0], &screenRow[terminalText->m_terminal.getMaxColumns()]};
        terminalText->m_terminalModel.setData(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset), QString::fromStdString(screenLine), Qt::DisplayRole);
        terminalText->m_terminal.scrollUp(0, terminalText->m_terminal.getMaxRows());
        terminalText->m_currentViewOffset++;
    }

    terminalText->setCurrentIndex(terminalText->m_terminalModel.index(terminalText->m_currentViewOffset+terminalText->m_terminal.getCurrentRow()-1));

}

/**
 * @brief QtTerminal::bufferToScreen
 *
 * Flush CTerminal buffer to ListView.
 *
 */
void QtTerminal::bufferToScreen()
{

    int currentRow=m_currentViewOffset;

    for (auto row=0; row < m_terminal.getMaxRows(); row++) {
        std::uint8_t*screenRow=m_terminal.getBuffer(0,row);
        const std::string screenLine { &screenRow[0], &screenRow[m_terminal.getMaxColumns()]};
        m_terminalModel.setData(m_terminalModel.index(currentRow++), QString::fromStdString(screenLine), Qt::DisplayRole);
    }

}

/**
 * @brief QtTerminal::keyPressEvent
 *
 * Send ASCII for keypress to server. Arrow keys are translated into the correct escape
 * sequence for the server.Note : More keys may need to be translated in future.
 *
 * @param event
 */
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

/**
 * @brief QtTerminal::terminalOutput
 *
 * Send recieved characters from remote shell to CTerminal.
 *
 * @param characters
 */
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
