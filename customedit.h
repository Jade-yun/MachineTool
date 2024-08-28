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

    int getDecimalPlaces() const;
    QVariant getMinValue() const;
    QVariant getMaxValue() const;

    QString formatInput(const QString& inputText) const;
public slots:
    void validateInput();
signals:
    void showRangeError(const QString& message);
    void finishedInput();

private:
    QString previousValue;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CUSTOMEDIT_H
