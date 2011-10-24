#ifndef EDITFEEDDIALOG_H
#define EDITFEEDDIALOG_H

#include <QDialog>
#include "feedmodel.h"

namespace Ui {
    class EditFeedDialog;
}

namespace Larss {

    class EditFeedDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit EditFeedDialog(QWidget *parent, FeedModel *feedModel);
        ~EditFeedDialog();

        /**
         * @brief Get the name of the New feed.
         */
        QString getFeedName();

        /**
         * @brief Get the URL selected by the user.
         */
        QString getFeedUrl();

        /**
         * @brief Return the ID of the selected category.
         */
        quint64 getCategoryId();

    private:
        Ui::EditFeedDialog *ui;
        FeedModel *feedModel;
    };

}

#endif // EDITFEEDDIALOG_H
