#ifndef HELICALSERVERCONNECTIONSDIALOG_H
#define HELICALSERVERCONNECTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class HelicalServerConnectionsDialog;
}

class HelicalServerConnectionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelicalServerConnectionsDialog(QWidget *parent = 0);
    ~HelicalServerConnectionsDialog();

    void loadConnectionsList();
    void populateConnectionList();
    void saveConnectionList();

signals:

    void connectToServer(const QString &connectionName);

private slots:

    void on_newServerButton_clicked();
    void on_editServerButton_clicked();
    void on_removeServerButton_clicked();
    void on_connectServerButton_clicked();

private:
    Ui::HelicalServerConnectionsDialog *ui;

    QStringList m_connectionList;

};

#endif // HELICALSERVERCONNECTIONSDIALOG_H
