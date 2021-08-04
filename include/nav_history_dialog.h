#ifndef NAV_HISTORY_DIALOG_H
#define NAV_HISTORY_DIALOG_H

#include <QDialog>

namespace Ui {
class NavHistoryDialog;
}

class NavHistoryDialog : public QDialog {
Q_OBJECT

public:
    explicit NavHistoryDialog(const QString &code, const QString &name, QWidget *parent = nullptr);

    ~NavHistoryDialog() override;

private slots:

    void get_more_history();


private:
    static QVariantMap get_json_from_networker(const QString &url);

    Ui::NavHistoryDialog *ui;

    const QString code;

    const QString name;

    int page_size = 30;

    int current_page = 1;

    int total_pages = 0;

    static const QString api_base_url;
};

#endif // NAV_HISTORY_DIALOG_H
