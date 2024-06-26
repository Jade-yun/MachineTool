#ifndef MANUALFORM_H
#define MANUALFORM_H

#include <QWidget>

namespace Ui {
class ManualForm;
}

class ManualForm : public QWidget
{
    Q_OBJECT

public:
    explicit ManualForm(QWidget *parent = nullptr);
    ~ManualForm();

private slots:

private:
    Ui::ManualForm *ui;
};

#endif // MANUALFORM_H
