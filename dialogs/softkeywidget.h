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
    bool moveEnable;
    QPoint mousePoint;


private:
    Ui::SoftKeyWidget *ui;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // SOFTKEYWIDGET_H
