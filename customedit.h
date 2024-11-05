#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include <QLineEdit>

class NumberEdit : public QLineEdit
{
    Q_OBJECT

public:
    NumberEdit(QWidget* parent = nullptr);

    void setDecimalPlaces(int places);
    void setInputRange(const QVariant& min = INT_MIN, const QVariant& max = INT_MAX);

    // be careful not to exceed the exact range that a double can represent
    // when inputing an integer.
    void setValue(const QVariant &newValue);

    int getDecimalPlaces() const;
    QVariant getMinValue() const;
    QVariant getMaxValue() const;

    QString formatInput(const QString& inputText) const;
private slots:
    void validateInput();
signals:
    void showRangeError(const QString& message);
    void finishedInput();

private:
    QString previousValue;
    QVariant value;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CUSTOMEDIT_H
