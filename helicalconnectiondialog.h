#ifndef HELICALCONNECTIONDIALOG_H
#define HELICALCONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class HelicalConnectionDialog;
}

class HelicalConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelicalConnectionDialog(QWidget *parent = 0);
    ~HelicalConnectionDialog();

private:
    Ui::HelicalConnectionDialog *ui;
};

#endif // HELICALCONNECTIONDIALOG_H
