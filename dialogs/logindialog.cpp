#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>

#include "errortipdialog.h"

extern uint32_t passwd[4]; // 0-管理员密码 1-超级管理密码 2-菜单密码,3-恢复出场设置密码
LoginMode LoginDialog::mode = LoginMode::Operator;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowTitle("Login");

    ui->editPasswd->setDecimalPlaces(0);
    ui->editPasswd->setInputRange(0, 99999);

    connect(ui->chboxOperator, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == Qt::Checked)
        {
            ui->editPasswd->setEnabled(false);
        }
        else
        {
            ui->editPasswd->setEnabled(true);
        }
    });

    connect(ui->btnCancle, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->btnOK, &QPushButton::clicked, this, &LoginDialog::onBtnOk);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

LoginMode LoginDialog::getLoginMode()
{
    return mode;
}

void LoginDialog::onBtnOk()
{
    LoginMode inputMode;
    uint32_t inputPasswd = ui->editPasswd->text().toUInt();

    if (ui->chboxOperator->isChecked())
    {
        inputMode = LoginMode::Operator;
    }
    else if (ui->chboxAdmin->isChecked())
    {
        if (!verifyPassword(inputPasswd, passwd[0]))
        {
            ui->editPasswd->clear();
            return;
        }
        inputMode = LoginMode::Admin;
    }
    else if (ui->chboxAdvance->isChecked())
    {
        if (!verifyPassword(inputPasswd, passwd[1]))
        {
            ui->editPasswd->clear();
            return;
        }
        inputMode = LoginMode::Advance;
    }

    if (mode != inputMode)
    {
        mode = inputMode;
        emit loginModeChanged(mode);
    }
    accept();
}

bool LoginDialog::verifyPassword(uint32_t inputPasswd, uint32_t correctPasswd)
{
    if (inputPasswd != correctPasswd) {
        ErrorTipDialog tip(tr("密码错误！"), TipMode::ONLY_OK);
        tip.exec();
        return false;
    }
    return true;
}

void LoginDialog::showEvent(QShowEvent *event)
{
    if (mode == LoginMode::Operator)
    {
        ui->chboxOperator->setChecked(true);
        ui->editPasswd->setEnabled(false);
    }
    else if (mode == LoginMode::Admin)
    {
        ui->chboxAdmin->setChecked(true);
    }
    else if (mode == LoginMode::Advance)
    {
        ui->chboxAdvance->setChecked(true);
    }

    ui->editPasswd->clear();

    if (parentWidget()) {
        QPoint center = parentWidget()->geometry().center();
        move(center - QPoint(this->width() / 2, this->height() / 2));
    }
    QWidget::showEvent(event);
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    QRect rect(0, 0, this->width(), this->height());

    QColor colorBackgroud("#24B4A5"); // 青绿色 (#24B4A5)
//    QColor colorBackgroud("#A5D6A7"); // 浅蓝绿色 (#A5D6A7)
//    QColor colorBackgroud("#2C3E50"); // 深蓝色
//    QColor colorBackground("#2D3E34"); // 哑光深绿色
    painter.fillRect(rect, colorBackgroud);
}
