#include "softkeywidget.h"
#include "ui_softkeywidget.h"

#include <QIcon>
#include <QPropertyAnimation>


const QString key_icons_path[6] = {
    ":/images/start.png",
    ":/images/stop.png",
    ":/images/origin.png",
    ":/images/revert.png",
    ":/images/last.png",
    ":/images/next.png",
};

SoftKeyWidget::SoftKeyWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftKeyWidget)
{
    ui->setupUi(this);

    this->setModal(false);

    QString buttonStyle = R"(
        QPushButton {
            background-color: transparent;
            outline: none;
            border: 1px;
        }
        QPushButton::pressed {
            background-color: transparent;
        }
        QPushButton::released {
            background-color: transparent;
        }
    )";

    this->setStyleSheet(buttonStyle);

    const std::array<QPushButton*, 6> buttons = {
        ui->btnStart, ui->btnStop, ui->btnOrigin, ui->btnRevert, ui->btnLast, ui->btnNext
    };

    QSize btnSize = QSize(64, 64);
    QSize smallerSize(btnSize.width() * 0.9, btnSize.height() * 0.9);

    for (int i = 0; i < 6; i++)
    {
        QPushButton *button = buttons[i];

        button->setFixedSize(btnSize);
        QIcon icon(key_icons_path[i]);
        button->setIcon(icon);
        button->setIconSize(btnSize);

        connect(button, &QPushButton::pressed, [button, icon, smallerSize]() {
            QPixmap pixmap = icon.pixmap(smallerSize);
            button->setIcon(QIcon(pixmap));
            button->setIconSize(smallerSize);
//            button->resize(smallerSize);
        });

        connect(button, &QPushButton::released, [button, icon, btnSize]() {
            QPixmap pixmap = icon.pixmap(btnSize);
            button->setIcon(QIcon(pixmap));
            button->setIconSize(btnSize);
//            button->resize(btnSize);
        });
    }


}

SoftKeyWidget::~SoftKeyWidget()
{
    delete ui;
}
