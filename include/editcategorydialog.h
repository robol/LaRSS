#ifndef EDITCATEGORYDIALOG_H
#define EDITCATEGORYDIALOG_H

#include <QDialog>

namespace Ui {
    class EditCategoryDialog;
}

namespace Larss {

    class EditCategoryDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit EditCategoryDialog(QWidget *parent = 0);
        ~EditCategoryDialog();

        /**
         * @brief Get the name of the category to be created.
         */
        QString getCategoryName();

    private:
        Ui::EditCategoryDialog *ui;
    };

}

#endif // EDITCATEGORYDIALOG_H
