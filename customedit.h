#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include <QLineEdit>

class NumberEdit : public QLineEdit
{
    Q_OBJECT

public:
    NumberEdit(QWidget* parent = nullptr);

    void setDecimalPlaces(int places);
    void setInputRange(const QVariant& min, const QVariant& max);

    int getDecimalPlaces() const;
    QVariant getMinValue() const;
    QVariant getMaxValue() const;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CUSTOMEDIT_H
