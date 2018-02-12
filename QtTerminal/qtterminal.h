/*
 * File:   qtterminal.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef QTTERMINALTEXT_H
#define QTTERMINALTEXT_H

//
// Class: QtTerminal
//
// Description: Class to act as Qt specific wrapper for class CTerminal. It is QListView that
// is used to display a QStringListModel into which the CTerminal buffer has been flushed. The
// keyPressEvent function is also overridden so that key stokes may be sent down the shell channel to
// the server. A more efficient way of doing at a later date will be found; through the use
// of a custom view and model class.
//

// =============
// INCLUDE FILES
// =============

#include <QtTerminal/cterminal.h>

#include <QObject>
#include <QListView>
#include <QStringListModel>
#include <QKeyEvent>

// =================
// CLASS DECLARATION
// =================

class QtTerminal : public QListView
{
    Q_OBJECT

public:

    // Constructor

    QtTerminal(int columns, int rows, QWidget *parent = 0);

    // Flush buffer to screen (ListView)

    void bufferToScreen();

    // Scroll screen (ListView) up n lines

    static void scrollScreenUp( void *termminalContext, int numberofLines);

protected:

    // Override to get keys pressed events

    virtual void keyPressEvent(QKeyEvent *e);

signals:

    // Send ASCII for any key pressed to server

    void keySend(const QByteArray &keyAscii);

public slots:

    // Send characters to CTerminal

    void terminalOutput(const QString &characters);

private:

    QStringListModel m_terminalModel;   // String List view model (Screen)
    int m_currentViewOffset {0};        // Offset for beginning of screen area
    CTerminal m_terminal;               // Vt100 terminal emulation

};

#endif // QTTERMINALTEXT_H
