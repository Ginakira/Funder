#include "include/settings_tab.h"
#include "ui/ui_settings_tab.h"

#include <QMessageBox>
#include <QFont>

#include "include/settings.h"

SettingsTab::SettingsTab(Settings *settings, QWidget *parent)
        : QWidget(parent), ui(new Ui::SettingsTab), settings(settings) {
    ui->setupUi(this);

    // 初始化窗口文字大小
    QFont font = QApplication::font();
    int font_size = settings->load_window_font_size();
    font.setPointSize(font_size);
    QApplication::setFont(font);
    ui->font_size_spin_box->setValue(font_size);

    // 文字大小修改数字框
    connect(ui->font_size_spin_box, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsTab::font_size_changed);

    connect(ui->about_button, &QPushButton::clicked, this, &SettingsTab::about_message);

}

SettingsTab::~SettingsTab() {
    delete ui;
}

void SettingsTab::about_message() {
    QMessageBox::about(this, tr("关于Funder"), tr(
            "Funder %1 %2\n"
            "一款基于 Qt 5 的跨平台基金信息统计工具\n\n"
            "本产品已使用GPLv3协议于Github上开源: \n"
            "https://github.com/Ginakira/Funder\n\n"
            "理财有风险，投资需谨慎，本产品所提供的信息仅供交流参考，任何所造成的经济后果和法律责任由使用者自行承担\n"
            "本产品所采用的基金净值、估值信息分别来自于蛋卷基金网和天天基金网\n\n"
            "Powered by Ginakira\n"
    ).arg(APP_VERSION, BUILD_VERSION));
}

void SettingsTab::font_size_changed(int font_size) {
    settings->save_window_font_size(font_size);
    QFont font = QApplication::font();
    font.setPointSize(settings->load_window_font_size());
    QApplication::setFont(font);
}
