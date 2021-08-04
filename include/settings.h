//
// Created by Sakata on 2021/6/24.
//

#ifndef FUNDER_SETTINGS_H
#define FUNDER_SETTINGS_H

#include <QObject>
#include <QSettings>

extern const char *APP_VERSION;
extern const char *BUILD_VERSION;

class Settings : public QObject {
Q_OBJECT;
public:
    explicit Settings(const QString &path, QObject *parent = nullptr);

    void save_horizontal_header_state_to_settings(const QByteArray &state);

    QByteArray load_horizontal_header_state_to_settings();

    void save_buy_service_charge(const QString &code, double service_charge);

    double load_buy_service_charge(const QString &code);

    void save_window_geometry(const QByteArray &state);

    QByteArray load_window_geometry();

    void save_window_font_size(int font_size);

    int load_window_font_size();

    void save_row_height(int height);

    int load_row_height();

    void save_main_background_color(const QString &hex_color);

    QString load_main_background_color();

    void save_secondary_background_color(const QString &hex_color);

    QString load_secondary_background_color();

    void reset_main_background_color();

    void reset_secondary_background_color();

private:
    QSettings *m_settings;
};


#endif //FUNDER_SETTINGS_H
