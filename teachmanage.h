#ifndef TEACHMANAGE_H
#define TEACHMANAGE_H

#include <QWidget>
#include "customkeyboard.h"

#define TESTKEYBOARD 0


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


private:
    void callKeyboard(QObject *watched);
private:
    bool isBtn_USB_Checked;

#if TESTKEYBOARD
    FullKeyboard* keyboard;

public slots:
    bool eventFilter(QObject *,QEvent *) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
#endif
};

#endif // TEACHMANAGE_H
