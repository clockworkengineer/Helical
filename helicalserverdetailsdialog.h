#ifndef HELICALSERVERDETAILSDIALOG_H
#define HELICALSERVERDETAILSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class HelicalServerDetailsDialog;
}

class HelicalServerDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelicalServerDetailsDialog(QWidget *parent = 0);
    ~HelicalServerDetailsDialog();

    QString connectionName() const;

private slots:
    void on_cancelButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::HelicalServerDetailsDialog *ui;

    QString m_connectionName;
    QString m_serverName;
    QString m_serverPort;
    QString m_userName;
    QString m_userPassword;

};

#endif // HELICALSERVERDETAILSDIALOG_H
