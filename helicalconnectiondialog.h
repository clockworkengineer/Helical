#ifndef HELICALCONNECTIONDIALOG_H
#define HELICALCONNECTIONDIALOG_H

#include <QDialog>
#include <QHBoxLayout>

#include "qtterminaltext.h"
#include "QtSSH/qtsshchannel.h"

namespace Ui {
class HelicalConnectionDialog;
}

class HelicalConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelicalConnectionDialog(QtSSH &session, QWidget *parent = 0);
    ~HelicalConnectionDialog();

    void runCommand(const QString &command);
    void runShell(int columns, int rows);

public slots:

    void keyRecv(const QByteArray &keyAscii);
    void remoteShellClosed();

public:

private:

    void setupTerminalTextArea();

    Ui::HelicalConnectionDialog *ui;

    QtTerminalText *m_terminalTextArea {nullptr};

    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr};
    QScopedPointer<QHBoxLayout> m_textAreaLayout {nullptr};
    QScopedPointer<std::thread> m_commandThread {nullptr};


};

#endif // HELICALCONNECTIONDIALOG_H
