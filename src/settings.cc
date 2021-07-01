//
// Created by Sakata on 2021/6/24.
//

#include "include/settings.h"

#define HORIZONTAL_HEADER_STATE_KEY "holding_tab/horizontal_header_state"
#define BUY_SERVICE_CHARGE "buy_service_charge" // 买入手续费
#define WINDOW_GEOMETRY "app/window_geometry" // 窗口大小和位置

Settings::Settings(const QString &path, QObject *parent) : QObject(parent) {
    m_settings = new QSettings(path, QSettings::IniFormat, parent);
}

void Settings::save_horizontal_header_state_to_settings(const QByteArray &state) {
    m_settings->setValue(HORIZONTAL_HEADER_STATE_KEY, state);

}

QByteArray Settings::load_horizontal_header_state_to_settings() {
    return m_settings->value(HORIZONTAL_HEADER_STATE_KEY).toByteArray();
}

void Settings::save_buy_service_charge(const QString &code, double service_charge) {
    m_settings->setValue(QString("%1/%2").arg(BUY_SERVICE_CHARGE, code), service_charge);
}

double Settings::load_buy_service_charge(const QString &code) {
    QVariant result = m_settings->value(QString("%1/%2").arg(BUY_SERVICE_CHARGE, code));
    if (!result.isValid()) {
        return 0;
    }
    return result.toDouble();
}

void Settings::save_window_geometry(const QByteArray &state) {
    m_settings->setValue(WINDOW_GEOMETRY, state);
}

QByteArray Settings::load_window_geometry() {
    return m_settings->value(WINDOW_GEOMETRY).toByteArray();
}
