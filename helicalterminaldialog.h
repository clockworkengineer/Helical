#ifndef HELICALCONNECTIONDIALOG_H
#define HELICALCONNECTIONDIALOG_H

#include <QDialog>
#include <QHBoxLayout>

#include "QtTerminal/qtterminal.h"
#include "QtSSH/qtsshchannel.h"

namespace Ui {
class HelicalConnectionDialog;
}

class HelicalTerminalDialog : public QDialog
{
    Q_OBJECT

public:

    explicit HelicalTerminalDialog(QtSSH &session, int columns, int rows, QWidget *parent = 0);
    ~HelicalTerminalDialog();

    void runCommand(const QString &command);
    void runShell();

public slots:

    void keyRecv(const QByteArray &keyAscii);
    void remoteShellClosed();

protected:

    void closeEvent(QCloseEvent *event);

private:

    void setupTerminalTextArea();
    void terminateShell();

    //void vt100Unsupported();

    Ui::HelicalConnectionDialog *ui;

    QtTerminal *m_terminalTextArea {nullptr};

    int m_columns {0};
    int m_rows {0};

    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr};
    QScopedPointer<QHBoxLayout> m_textAreaLayout {nullptr};
    QScopedPointer<std::thread> m_remoteShellThread {nullptr};

};

#endif // HELICALCONNECTIONDIALOG_H
