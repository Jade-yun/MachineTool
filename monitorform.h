#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class MonitorForm;
}

class MonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorForm(QWidget *parent = nullptr);
    ~MonitorForm();

private:
    Ui::MonitorForm *ui;

    void setLED(QLabel* label, int color, int size);

    QVector<QLabel*> v_lab;
};

#endif // MONITORFORM_H
