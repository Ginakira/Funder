#ifndef SETTINGS_TAB_H
#define SETTINGS_TAB_H

#include <QWidget>
#include "include/settings.h"

namespace Ui {
class SettingsTab;
}

class SettingsTab : public QWidget {
Q_OBJECT

public:
    explicit SettingsTab(Settings *settings, QWidget *parent = nullptr);

    ~SettingsTab() override;

public slots:

    void about_message();

    void font_size_changed(int font_size);

signals:

    void row_height_changed(int height);

private:
    Ui::SettingsTab *ui;
    Settings *settings;
};

#endif // SETTINGS_TAB_H
