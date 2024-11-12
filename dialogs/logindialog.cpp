#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    mode(LoginMode::Operator), passwd(0)
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

int LoginDialog::getLoginMode() const
{
    return mode;
}

uint LoginDialog::getInputPasswd() const
{
    return passwd;
}

void LoginDialog::onBtnOk()
{
    if (ui->chboxOperator->isChecked())
    {
        mode = LoginMode::Operator;
        passwd = 0;
    }
    else if (ui->chboxAdmin->isChecked())
    {
        mode = LoginMode::Admin;
        passwd = ui->editPasswd->text().toUInt();
    }
    else if (ui->chboxAdvance->isChecked())
    {
        mode = LoginMode::Advance;
        passwd = ui->editPasswd->text().toUInt();
    }
    accept();
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
    passwd = 0;

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
