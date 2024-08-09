#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include "QLabel"
#include "QLineEdit"
#include "QComboBox"
#include "QPushButton"
#include "QTableWidgetItem"
//#include <QList>

#include "customkeyboard.h"
#include "method/iniconfig.h"

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private:

    void pageSwitchInit();
    void machineParaLogic();

public slots:
    void slotSettingHome();
    void ShowStackPage();

private:
    Ui::Setting *ui;

private:
    void initVariables();
    void readFromConfigFile();
    void writeToConfigFile();

};

#endif // SETTING_H
