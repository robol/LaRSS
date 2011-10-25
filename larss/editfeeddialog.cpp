#include "editfeeddialog.h"
#include "ui_editfeeddialog.h"
#include "feedmodel.h"

using namespace Larss;

EditFeedDialog::EditFeedDialog(QWidget *parent, FeedModel *feedModel) :
    QDialog(parent),
    ui(new Ui::EditFeedDialog)
{
    this->feedModel = feedModel;
    ui->setupUi(this);

    // Load model in the ComboBox
    ui->categoryComboBox->setModel(feedModel);
}

EditFeedDialog::~EditFeedDialog()
{
    delete ui;
}

QString
EditFeedDialog::getFeedName()
{
    return ui->newFeedName->text();
}

QString
EditFeedDialog::getFeedUrl()
{
    return ui->newFeedUrl->text();
}

FeedNode*
EditFeedDialog::getCategory()
{
    QModelIndex root = feedModel->indexFromItem(feedModel->invisibleRootItem());
    QModelIndex selectedCategory = feedModel->index (ui->categoryComboBox->currentIndex(),
                                                     0, root);
    return feedModel->itemFromIndex(selectedCategory);
}
