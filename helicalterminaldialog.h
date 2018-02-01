#ifndef HELICALCONNECTIONDIALOG_H
#define HELICALCONNECTIONDIALOG_H

#include <QDialog>
#include <QHBoxLayout>

#include "qtterminaltext.h"
#include "QtSSH/qtsshchannel.h"

namespace Ui {
class HelicalConnectionDialog;
}

class HelicalTerminalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelicalTerminalDialog(QtSSH &session, QWidget *parent = 0);
    ~HelicalTerminalDialog();

    void runCommand(const QString &command);
    void runShell(int columns, int rows);

public slots:

    void keyRecv(const QByteArray &keyAscii);
    void remoteShellClosed();

protected:
    void closeEvent(QCloseEvent *event);

private:

    void setupTerminalTextArea();
    void terminateShell();

    Ui::HelicalConnectionDialog *ui;

    QtTerminalText *m_terminalTextArea {nullptr};

    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr};
    QScopedPointer<QHBoxLayout> m_textAreaLayout {nullptr};
    QScopedPointer<std::thread> m_remoteShellThread {nullptr};

};

#endif // HELICALCONNECTIONDIALOG_H
