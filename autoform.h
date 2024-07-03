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

private slots:
    void on_btnNext_clicked();

    void on_btnLast_clicked();


private:
    void updateLabelState(int index);
private:
    Ui::AutoForm *ui;

};

#endif // AUTOFORM_H
