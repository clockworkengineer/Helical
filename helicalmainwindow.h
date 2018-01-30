#ifndef HEILCALMAINWINDOW_H
#define HEILCALMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class HeilcalMainWindow;
}

class HeilcalMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HeilcalMainWindow(QWidget *parent = 0);
    ~HeilcalMainWindow();

private:
    Ui::HeilcalMainWindow *ui;
};

#endif // HEILCALMAINWINDOW_H
