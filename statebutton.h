#ifndef STATEBUTTON_H
#define STATEBUTTON_H

#include <QPushButton>
#include <QIcon>

class StateButton : public QPushButton
{
    Q_OBJECT

public:
    StateButton(QWidget* parent = nullptr);

private slots:
    void toggleConnection();

public:
    void setObjectText(const QString& text);


private:
    bool connected;  // 记录按钮的连接状态
    QString objectText;

    void updateState();
};

#endif // STATEBUTTON_H
