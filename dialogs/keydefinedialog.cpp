#include "keydefinedialog.h"
#include "ui_keydefinedialog.h"

#include <QDebug>

KeyDefineDialog::KeyDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyDefineDialog)
{
    ui->setupUi(this);

    connect(ui->btnOK, &QPushButton::clicked, [=](){
        this->close();
    });
    connect(ui->btnCancel, &QPushButton::clicked, [=](){
        this->close();
    });
}

KeyDefineDialog::~KeyDefineDialog()
{
    delete ui;
}


#if USE_LINEEDIT

KeyEdit::KeyEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void KeyEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
//    qDebug() << "KeyEdit is clicked ";
    KeyDefineDialog dialog(this);
    dialog.exec();
}

#else
KeyEdit::KeyEdit(QWidget *parent)
    : QLabel(parent)
{
    setStyleSheet(
        "QLabel {"
        "   background-color: white;"
        "   border: 1px solid #A0A0A0;"
        "   border-radius: 2px;"
        "   padding: 2px;"
        "   color: black;"
        "   min-height: 40px;"
        "}"
    );
}

void KeyEdit::mousePressEvent(QMouseEvent *event)
{
//    QLineEdit::mousePressEvent(event);
    qDebug() << "KeyEdit is clicked ";
}
#endif
