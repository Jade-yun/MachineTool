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


private slots:
    void on_btn1_cut_clicked();
    void on_btn1_add_clicked();
	void on_btn2_cut_clicked();
	void on_btn2_add_clicked();
	void on_btn3_cut_clicked();
	void on_btn3_add_clicked();
	void on_btn4_cut_clicked();
	void on_btn4_add_clicked();
	void on_btn12_cut_clicked();
	void on_btn12_add_clicked();

    void on_btnMerge_Expand_clicked();
};

#endif // MONITORFORM_H
