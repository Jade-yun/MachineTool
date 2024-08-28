#ifndef YES_NO_PAGE_H
#define YES_NO_PAGE_H

#include <QWidget>

namespace Ui {
class yes_no_page;
}

class yes_no_page : public QWidget
{
    Q_OBJECT

public:
    explicit yes_no_page(QWidget *parent = 0);
    ~yes_no_page();
    Ui::yes_no_page *ui;
private slots:
    void on_Key_yes_no_page_yes_clicked();

    void on_Key_yes_no_page_no_clicked();

private:

};

#endif // YES_NO_PAGE_H
