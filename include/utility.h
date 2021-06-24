#ifndef UTILITY_H
#define UTILITY_H

#include <QString>

QString getDecodedString(const QByteArray &ba);

void init_settings_info(const QString &org_name, const QString &app_name);

void save_horizontal_header_state_to_settings(const QByteArray &state);

QByteArray load_horizontal_header_state_to_settings();

#endif  // UTILITY_H
