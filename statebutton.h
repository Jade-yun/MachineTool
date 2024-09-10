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
    void setStateTexts(const QStringList& texts);
    void setStateTexts(const QString &connectedText, const QString &disconnectedText);
    bool getState() const;

private:
    bool connected;  // 记录按钮的连接状态
    QString objectText;
    QString connectedText;
    QString disconnectedText;

    void updateState();
};

#endif // STATEBUTTON_H
