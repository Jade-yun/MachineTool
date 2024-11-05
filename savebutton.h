#ifndef SAVEBUTTON_H
#define SAVEBUTTON_H

#include <QPushButton>
#include <QTimer>

class SaveButton : public QPushButton {
    Q_OBJECT

public:
    explicit SaveButton(QWidget* parent = nullptr);

    void setParaChangedFlag(bool flag);
    bool isParaChanged() const;

private slots:
    void onTimeout();

private:
    void startFlashing();
    void stopFlashing();

    QTimer* timer;
    bool paraChanged;

    QIcon saveIcon;
    QIcon emptyIcon;
};

#endif // SAVEBUTTON_H
