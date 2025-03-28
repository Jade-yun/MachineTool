#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include <QLineEdit>

class NumberEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit NumberEdit(QWidget* parent = nullptr);

    void setDecimalPlaces(int places);
    void setInputRange(double min, double max);
    void setValue(double newValue);

    int getDecimalPlaces() const;
    double getMinValue() const;
    double getMaxValue() const;
//    double getValue() const;

    QString formatInput(const QString& inputText) const;

private slots:
    void validateInput();

signals:
    void showRangeError(const QString& message);
    void finishedInput();

private:
    double minValue;
    double maxValue;
    double value;
    double previousValue;
    int decimalPlaces;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CUSTOMEDIT_H
