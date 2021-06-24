#include "include/utility.h"
#include <QTextCodec>
#include <QSettings>
#include <QCoreApplication>

#define HORIZONTAL_HEADER_STATE_KEY "holding_tab/horizontal_header_state"

// 将QByteArray使用GBK字符集转换为QString
QString getDecodedString(const QByteArray &ba) {
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    return codec->toUnicode(ba);
}

// 初始化设置信息
void init_settings_info(const QString &org_name, const QString &app_name) {
    QCoreApplication::setOrganizationName(org_name);
    QCoreApplication::setApplicationName(app_name);
}

// 存储表头列顺序、状态
void save_horizontal_header_state_to_settings(const QByteArray &state) {
    QSettings settings;
    settings.setValue(HORIZONTAL_HEADER_STATE_KEY, state);
}

// 读取表头列顺序、状态
QByteArray load_horizontal_header_state_to_settings() {
    QSettings settings;
    return settings.value(HORIZONTAL_HEADER_STATE_KEY).toByteArray();
}
