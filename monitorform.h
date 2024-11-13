#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QThread>
namespace Ui {
class MonitorForm;
}

class MonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorForm(QWidget *parent = nullptr);
    ~MonitorForm();
    void InitAllLedName();
private:
    Ui::MonitorForm *ui;
    void setLED(QLabel* label, int color, int size);
    void RefreshLedState();
    void setLedState(QLabel* label, uint8_t port,uint8_t type);
    void setLedName(QLabel* label, uint8_t port,uint8_t type);
    QTimer *RefreshLedTime = nullptr;
private slots:
    void on_btn1_cut_clicked();
    void on_btn1_add_clicked();
	void on_btn2_cut_clicked();
	void on_btn2_add_clicked();
    void on_btn1_cut_2_clicked();
    void on_btn1_add_2_clicked();
    void on_btn2_cut_2_clicked();
    void on_btn2_add_2_clicked();
    void on_btnInWIdget_cut_clicked();
    void on_btnInWIdget_add_clicked();
    void on_btnOutWidget_cut_clicked();
    void on_btnOutWidget_add_clicked();

    void on_btnMerge_Expand_clicked();
};

#endif // MONITORFORM_H
