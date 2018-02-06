#ifndef QTTERMINALTEXT_H
#define QTTERMINALTEXT_H

#include <QtTerminal/cterminal.h>

#include <QObject>
#include <QListView>
#include <QStringListModel>
#include <QKeyEvent>

class QtTerminal : public QListView
{
    Q_OBJECT

public:

    QtTerminal(int columns, int rows, QWidget *parent = 0);

    void bufferToScreen();
    static void scrollScreenUp( void *termminalText, int numberofLines);

protected:

    virtual void keyPressEvent(QKeyEvent *e);

signals:

    void keySend(const QByteArray &keyAscii);

public slots:

    void terminalOutput(const QString &text);

private:

    QStringListModel m_terminalModel;
    int m_currentViewOffset {0};
    CTerminal m_terminal;


};

#endif // QTTERMINALTEXT_H
