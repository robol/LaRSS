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

quint64
EditFeedDialog::getCategoryId()
{
    return ui->categoryComboBox->currentIndex() + 1;
}
