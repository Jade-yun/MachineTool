#ifndef SOFTKEYWIDGET_H
#define SOFTKEYWIDGET_H

#include <QDialog>

namespace Ui {
class SoftKeyWidget;
}

class SoftKeyWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SoftKeyWidget(QWidget *parent = nullptr);
    ~SoftKeyWidget();

private:
    Ui::SoftKeyWidget *ui;
};

#endif // SOFTKEYWIDGET_H
