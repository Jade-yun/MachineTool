#ifndef TEACHMANAGE_H
#define TEACHMANAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QFileDialog>

#include "customkeyboard.h"
#include "cmd.h"

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

    QFileDialog* fileDialog;

signals:
    void programLoaded();//刷新新程序参考点信号

private slots:
    void on_btn_New_clicked();
    void on_btn_Copy_clicked();
    void on_btn_Load_clicked();
    void on_btn_Preview_clicked();
    void on_btn_Del_clicked();

    void on_btn_SelectAll_clicked();
    void on_btn_SelectNotAll_clicked();
    void on_btn_Export_clicked();
    void on_btn_Import_clicked();

private slots:
    void on_tableTeachManage_itemSelectionChanged();

    void on_chboxAllow_clicked();
    void on_chboxTimeAndPositionAllow_clicked();
    void on_chboxTimeAllow_clicked();
    void on_chboxForbide_clicked();

private:
    void init();
    void initWidgets();
    void deleteOneRowFromTable(QTableWidget *tableTeachManage);
    void addNewRowToTable(const QString& programName, const QString& fileTime, int permission);
    void changeProgramPermission(const QString* programName, int permission);

    void setFilePermision(QWidget *lab, const QVariant& permission);
    int getFilePermission(QWidget *lab) const;

    void updateFilePermission(int index);

signals:
    void labProgramNameChangeSignal();//主界面名称刷新信号
private:
    // 当前选中文件权限 0 允许    1 时间/位置允许   2 时间允许  3 禁止
    int filePermission;
};

#endif // TEACHMANAGE_H
