#ifndef BACKGROUNDPROCESSFORM_H
#define BACKGROUNDPROCESSFORM_H

#include <QDialog>

#include "QtUi/framelesswidget2.h"

namespace Ui {
class BackgroundProcessForm;
}

class BackgroundProcessForm : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundProcessForm(QWidget *parent = nullptr);
    ~BackgroundProcessForm();

private:
    Ui::BackgroundProcessForm *ui;

    FramelessWidget2* frameBase;
};

#endif // BACKGROUNDPROCESSFORM_H
