#include "include/utility.h"
#include <QTextCodec>
#include <QSettings>
#include <QCoreApplication>


// 将QByteArray使用GBK字符集转换为QString
QString getDecodedString(const QByteArray &ba) {
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    return codec->toUnicode(ba);
}
