#include "softkeywidget.h"
#include "ui_softkeywidget.h"

#include <QIcon>
#include <QPropertyAnimation>
#include <QHoverEvent>

#include "QtUi/framelesswidget2.h"


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
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

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
        #titleBar{background:#BBBBBB;}
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

    ui->labTitle->setText(tr("soft key"));
    this->setWindowTitle(ui->labTitle->text());
//    this->setTitleBar(ui->labTitle);


    FramelessWidget2 *framless = new FramelessWidget2(this);

    framless->setWidget(this);

    mousePoint = QPoint(0, 0);
//    this->installEventFilter(this);
}

SoftKeyWidget::~SoftKeyWidget()
{
    delete ui;
}

bool SoftKeyWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this)
    {
        if (event->type() == QEvent::MouseButtonPress) {
            //记住鼠标按下的坐标+窗体区域
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
//            mousePoint = mapToGlobal(mouseEvent->pos());
            mousePoint = mouseEvent->pos();
        }
        else if (event->type() == QEvent::MouseMove) {
            //设置对应鼠标形状,这个必须放在这里而不是下面,因为可以在鼠标没有按下的时候识别
//            QHoverEvent *hoverEvent = (QHoverEvent *)event;
//            QPoint point = mapToGlobal(hoverEvent->pos());
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QPoint point = mouseEvent->pos();

            //根据当前鼠标位置,计算XY轴移动了多少
            int offsetX = point.x() - mousePoint.x();
            int offsetY = point.y() - mousePoint.y();

            if (ui->titleBar->rect().contains(mousePoint))
            {
                this->move(this->x() + offsetX, this->y() + offsetY);
                this->update();
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}
