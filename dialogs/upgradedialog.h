#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H
#include "customkeyboard.h"
#include <QDialog>
#include "basewindow.h"
#include <QFileSystemModel>
#include <QStorageInfo>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QThread>
#include "errortipdialog.h"
enum upgrade_type {
    HANDHELD,
    MAINBOARD,
    IOBOARD,
    SERVO,
    SYSTEM_DATA_COPY,
    COPY_DATA_REST,
    RESTSETTING
};
namespace Ui {
class upgradedialog;
}

class upgradedialog : public BaseWindow
{
    Q_OBJECT

public:
    explicit upgradedialog(const uint8_t &upgradetype, QWidget *parent = nullptr);
    ~upgradedialog();

private slots:
    void on_Key_File_hand_select_clicked();

    void on_file_listView_doubleClicked(const QModelIndex &index);

    void on_toolButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_cut_button_clicked();

    void on_copy_button_clicked();

    void on_paste_button_clicked();

    void on_ok_button_clicked();

    void on_new_button_clicked();

    void on_delete_button_clicked();
    void on_Rename_button_clicked();

private:
    uint8_t UpgradeType = HANDHELD;
    Ui::upgradedialog *ui;
private:
    FullKeyboard* keyboard;
    ErrorTipDialog* dlgErrorTip;
    QStringList listOfFilesToCopy;
    QFileSystemModel *FileSystemModel;
    QSortFilterProxyModel *proxyModel;
    QStorageInfo storage;
    bool cutOperation; // 剪切状态
    QString UpFilePath = "/run/media/sdb1";
    QString destFilePath = "/opt/MachineTool/bin/updatafile/";
    bool Cover_SameNameFile = false;
public:
    int Upgrade_Progress = 0;//升级进度
    uint8_t sure_handle_type;//确定执行类型
private:
    bool hasDuplicateFileNames(const QString& dir1, const QString& dir2);
    QString findFileAndGetDirectory(const QString &usbPath, const QString &fileName);
    int showTip(const QString &message,TipMode mode = TipMode::NORMAL);
    void upgreadok_handle();
    void upgradelabel_handle(uint8_t Up_type);
    void updatePath(const QModelIndex &index);
    void copyFileOrFolder();
    void pasteFileOrFolder();
    void copyPath(QString src, QString dst);
    void Updata_handle();
    bool verifyHash(const QString &filePath, const QByteArray &expectedHash);
    void Upgrade_Progress_send(uint8_t progress);
    QByteArray expectedHash = QByteArray::fromHex("YOUR_EXPECTED_HASH_HERE");
protected:
    QString generateUniqueFileName(const QString &directory, const QString &baseName, const QString &extension);
signals:
    void updataWidgetShowSignal();//升级进度提示界面交互信号
    void UpProgressRefreshSignal();//更新进度信号
    void ImplementFinishSignal();//操作执行完成信号
};

#endif // UPGRADEDIALOG_H
