//
// Created by Sakata on 2021/6/24.
//

#ifndef FUNDER_SETTINGS_H
#define FUNDER_SETTINGS_H

#include <QObject>
#include <QSettings>

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

private:
    QSettings *m_settings;
};


#endif //FUNDER_SETTINGS_H
