//
// Created by Sakata on 2021/6/24.
//

#include "include/settings.h"

#include <QDebug>

#define HORIZONTAL_HEADER_STATE_KEY "holding_tab/horizontal_header_state"
#define BUY_SERVICE_CHARGE "buy_service_charge" // 买入手续费
#define WINDOW_GEOMETRY "app/window_geometry" // 窗口大小和位置
#define WINDOW_FONT_SIZE "app/font_size" // 窗口文字大小
#define ROW_HEIGHT "app/row_height" // 表格行高
#define MAIN_BACKGROUND_COLOR "app/main_background_color" // 主要背景色
#define SECONDARY_BACKGROUND_COLOR "app/secondary_background_color" // 次要背景色

const char *APP_VERSION = "1.23";
const char *BUILD_VERSION = "build 20210804";

#ifdef Q_OS_MACOS
#define DEFAULT_WINDOW_FONT_SIZE 13
#endif

#ifdef  Q_OS_WIN
#define DEFAULT_WINDOW_FONT_SIZE 10
#endif

#define DEFAULT_ROW_HEIGHT 30

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

void Settings::save_window_font_size(int font_size) {
    m_settings->setValue(WINDOW_FONT_SIZE, font_size);
}

int Settings::load_window_font_size() {
    return m_settings->value(WINDOW_FONT_SIZE, DEFAULT_WINDOW_FONT_SIZE).toInt();
}

void Settings::save_row_height(int height) {
    m_settings->setValue(ROW_HEIGHT, height);
}

int Settings::load_row_height() {
    return m_settings->value(ROW_HEIGHT, DEFAULT_ROW_HEIGHT).toInt();
}

void Settings::save_main_background_color(const QString &hex_color) {
    m_settings->setValue(MAIN_BACKGROUND_COLOR, hex_color);
}

QString Settings::load_main_background_color() {
    return m_settings->value(MAIN_BACKGROUND_COLOR).toString();
}

void Settings::save_secondary_background_color(const QString &hex_color) {
    m_settings->setValue(SECONDARY_BACKGROUND_COLOR, hex_color);
}

QString Settings::load_secondary_background_color() {
    return m_settings->value(SECONDARY_BACKGROUND_COLOR).toString();
}

void Settings::reset_main_background_color() {
    m_settings->remove(MAIN_BACKGROUND_COLOR);
}

void Settings::reset_secondary_background_color() {
    m_settings->remove(SECONDARY_BACKGROUND_COLOR);
}

