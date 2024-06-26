#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QWidget>

namespace Ui {
class AutoForm;
}

class AutoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutoForm(QWidget *parent = nullptr);
    ~AutoForm();

private:
    Ui::AutoForm *ui;

};

#endif // AUTOFORM_H
