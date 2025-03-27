#ifndef PRODUCTHISTORYDIALOG_H
#define PRODUCTHISTORYDIALOG_H

#include <QDialog>
#include <QTimer>
#include "ProductHistoryModel.h"

namespace Ui {
class ProductHistoryDialog;
}

class ProductHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductHistoryDialog(QWidget *parent = nullptr);
    ~ProductHistoryDialog();

    // 私有 method
private:
    void checkProductNum();
    // ui成员变量
private:
    Ui::ProductHistoryDialog *ui;

    // 其他成员变量
private:
    ProductHistoryModel *m_productHistoryModel;
    QTimer *m_timer;
};

#endif // PRODUCTHISTORYDIALOG_H
