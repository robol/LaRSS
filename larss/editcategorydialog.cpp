#include "include/editcategorydialog.h"
#include "ui_editcategorydialog.h"

using namespace Larss;

EditCategoryDialog::EditCategoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCategoryDialog)
{
    ui->setupUi(this);
}

EditCategoryDialog::~EditCategoryDialog()
{
    delete ui;
}

QString
EditCategoryDialog::getCategoryName()
{
    return ui->newCategoryName->text();
}
