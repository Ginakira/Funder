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

    void change_main_background_button_clicked();

    void change_secondary_background_button_clicked();

signals:

    void row_height_changed(int height);

    void main_background_color_changed(const QString &hex_color);

    void secondary_background_color_changed(const QString &hex_color);

private:
    Ui::SettingsTab *ui;
    Settings *settings;
};

#endif // SETTINGS_TAB_H
