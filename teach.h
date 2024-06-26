#ifndef TEACH_H
#define TEACH_H

#include <QWidget>

namespace Ui {
class Teach;
}

class Teach : public QWidget
{
    Q_OBJECT

public:
    explicit Teach(QWidget *parent = nullptr);
    ~Teach();

private:
    Ui::Teach *ui;
};

#endif // TEACH_H
