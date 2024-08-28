#ifndef PROMPT_DIALOG_PAGE_H
#define PROMPT_DIALOG_PAGE_H

#include <QDialog>

namespace Ui {
class Prompt_Dialog_page;
}

class Prompt_Dialog_page : public QDialog
{
    Q_OBJECT

public:
    explicit Prompt_Dialog_page(QWidget *parent = 0);
    ~Prompt_Dialog_page();
    Ui::Prompt_Dialog_page *ui;
private slots:
    void on_Key_Prompt_dialog_yes_clicked();
private:

};

#endif // PROMPT_DIALOG_PAGE_H
