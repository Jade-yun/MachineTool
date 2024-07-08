#ifndef TEACHMANAGE_H
#define TEACHMANAGE_H

#include <QWidget>

namespace Ui {
class TeachManage;
}

class TeachManage : public QWidget
{
    Q_OBJECT

public:
    explicit TeachManage(QWidget *parent = nullptr);
    ~TeachManage();

private:
    Ui::TeachManage *ui;

public slots:
    void on_btn_USB_clicked();

    void on_btn_New_clicked();
    void on_btn_Copy_clicked();
    void on_btn_Load_clicked();
    void on_btn_Preview_clicked();
    void on_btn_Del_clicked();

    void on_btn_SelectAll_clicked();
    void on_btn_SelectNotAll_clicked();
    void on_btn_Export_clicked();
    void on_btn_Import_clicked();

public slots:
//    bool eventFilter(QObject *,QEvent *);
private:
    bool isBtn_USB_Checked;

};

#endif // TEACHMANAGE_H
