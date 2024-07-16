﻿#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include "QLabel"
#include "QLineEdit"
#include "QComboBox"
#include "QPushButton"
#include "QTableWidgetItem"
//#include <QList>

#include "customkeyboard.h"


#define TESTKEYBOARD 0

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private slots:
    void on_btnSigSet_clicked();

    void on_btnSafetySet_clicked();

    void on_btnProductSet_clicked();

    void on_btnSystemSet_clicked();

    void on_btnServoSpeed_clicked();

    void on_btnServoSafePoint_clicked();

    void on_btnMachinePara_clicked();

    void on_btnStackSet_clicked();

    void on_comboBoxIotSelection_currentIndexChanged(int index);

    void on_comboBox_96_currentIndexChanged(int index);

private:

    void pageSwitchInit();

public slots:
    void slotSettingHome();

public slots:
    void onComboBoxIndexChanged(int index);
    void onPushButtonChanged(int index);
    void onPushButtonHeadChanged(int index);
    void onPushButtonGeneralClicked(int index);
private:
    void setAllStyleSheet();
    QPushButton* tb_1_btn[9];
    QPushButton* tb_1_btnHead[3];
    QComboBox* tb_1_comboBox[3];
    QLineEdit* tb_1_lineEdit[3][9];
    QPushButton* tb_general_btn;


private:
    Ui::Setting *ui;

private:
    void loadAndPlayGif(const QString& path);
    void initVariables();
#if TESTKEYBOARD
//    void showKeyboard();
    void showKeyboard(QLineEdit* edit);
    // QWidget interface
    KeyboardWindow* keyboard;
    QList<QLineEdit*> lineEdits;
protected:
    void keyPressEvent(QKeyEvent *event) override;

#endif

private:
    QMovie *movie;

};

#endif // SETTING_H
