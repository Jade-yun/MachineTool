#ifndef SETTING_H
#define SETTING_H

#include <QWidget>

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

    void on_btnLastPage_clicked();

    void on_btnNextPage_clicked();

    void on_comboBox_96_currentIndexChanged(int index);

public slots:
    void slotSettingHome();


private:
    Ui::Setting *ui;

private:
    void loadAndPlayGif(const QString& path);
    void setAllStyleSheet();
    void initVariables();

    QMovie *movie;
};

#endif // SETTING_H
