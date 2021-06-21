#include "include/fund_info.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlError>

#include "include/networker.h"
#include "include/utility.h"

FundInfo::FundInfo(QSqlTableModel *model, QObject *parent) : QObject(parent), model(model) {}

FundInfo::FundInfo(QSqlTableModel *model, const QSqlRecord &record, QObject *parent) : QObject(parent), model(model) {
    set_info_from_record(record);
}

const QString &FundInfo::get_code() const {
    return code;
}

void FundInfo::set_code(const QString &code) {
    FundInfo::code = code;
}

const QString &FundInfo::get_name() const {
    return name;
}

void FundInfo::set_name(const QString &name) {
    FundInfo::name = name;
}

double FundInfo::get_holding_unit_cost() const {
    return holding_unit_cost;
}

void FundInfo::set_holding_unit_cost(double holding_unit_cost) {
    FundInfo::holding_unit_cost = holding_unit_cost;
}

double FundInfo::get_holding_share() const {
    return holding_share;
}

void FundInfo::set_holding_share(double holding_share) {
    FundInfo::holding_share = holding_share;
}

double FundInfo::get_holding_total_cost() const {
    return holding_total_cost;
}

void FundInfo::set_holding_total_cost(double holding_total_cost) {
    FundInfo::holding_total_cost = holding_total_cost;
}

double FundInfo::get_holding_earnings() const {
    return holding_earnings;
}

void FundInfo::set_holding_earnings(double holding_earnings) {
    FundInfo::holding_earnings = holding_earnings;
}

double FundInfo::get_holding_earning_rate() const {
    return holding_earning_rate;
}

void FundInfo::set_holding_earning_rate(double holding_earning_rate) {
    FundInfo::holding_earning_rate = holding_earning_rate;
}

double FundInfo::get_holding_amount() const {
    return holding_amount;
}

void FundInfo::set_holding_amount(double holding_amount) {
    FundInfo::holding_amount = holding_amount;
}

double FundInfo::get_nav() const {
    return nav;
}

void FundInfo::set_nav(double nav) {
    FundInfo::nav = nav;
}

const QString &FundInfo::get_nav_time() const {
    return nav_time;
}

void FundInfo::set_nav_time(const QString &nav_time) {
    FundInfo::nav_time = nav_time;
}

double FundInfo::get_nav_gains() const {
    return nav_gains;
}

void FundInfo::set_nav_gains(double nav_gains) {
    FundInfo::nav_gains = nav_gains;
}

double FundInfo::get_valuation() const {
    return valuation;
}

void FundInfo::set_valuation(double valuation) {
    FundInfo::valuation = valuation;
}

double FundInfo::get_valuation_gains() const {
    return valuation_gains;
}

void FundInfo::set_valuation_gains(double valuation_gains) {
    FundInfo::valuation_gains = valuation_gains;
}

QString FundInfo::get_valuation_time() const {
    return valuation_time;
}

void FundInfo::set_valuation_time(const QString &valuation_time) {
    FundInfo::valuation_time = valuation_time;
}

double FundInfo::get_expected_earnings() const {
    return expected_earnings;
}

void FundInfo::set_expected_earnings(double expected_earnings) {
    FundInfo::expected_earnings = expected_earnings;
}

const QString &FundInfo::get_remarks() const {
    return remarks;
}

void FundInfo::set_remarks(const QString &remarks) {
    FundInfo::remarks = remarks;
}


QVariantMap
FundInfo::get_json_from_networker(const QString &url, const std::function<QString(const QString &)> &filter) {
    NetWorker *networker = NetWorker::instance();
    QNetworkReply *reply = networker->get(url);

    QEventLoop synchronous;
    connect(reply, &QNetworkReply::finished, &synchronous, &QEventLoop::quit);
    synchronous.exec();

    QString content = reply->readAll();
    if (filter != nullptr) {
        content = filter(content);
        if (content.isEmpty()) {
            return QVariantMap();
        }
    }

    QJsonParseError error{};
    QJsonDocument json_doc = QJsonDocument::fromJson(content.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qFatal("%s", error.errorString().toUtf8().constData());
    }

    QVariantMap result = json_doc.toVariant().toMap();

    reply->deleteLater();
    return result;
}

bool FundInfo::get_name_and_valuation_info() {
    if (code.isEmpty()) {
        QMessageBox::critical(nullptr, tr("基金基本信息和估值信息获取错误"), tr("基金代码为空，获取失败！"));
        return false;
    }

    const QString base_url = "https://fundgz.1234567.com.cn/js/%1.js";

    auto filter = [](const QString &content) -> QString {
        QRegExp regex("jsonpgz\\((.*)\\)");
        int pos = regex.indexIn(content);
        if (pos < 0) {
            qDebug() << "Regex error" << __FILE__ << __LINE__;
            return "";
        }
        return regex.cap(1);
    };

    QVariantMap result = get_json_from_networker(base_url.arg(code), filter);
    if (result.empty()) {
        return false;
    }
    name = result["name"].toString();
//    净值信息改为从蛋卷接口获取
//    nav_time = result["jzrq"].toString();
//    nav = result["dwjz"].toDouble();
    valuation = result["gsz"].toDouble();
    valuation_gains = result["gszzl"].toDouble();
    valuation_time = result["gztime"].toString();
    return true;
}

bool FundInfo::get_nav_info() {
    if (code.isEmpty()) {
        QMessageBox::critical(nullptr, tr("基金净值信息获取错误"), tr("基金代码为空，获取失败！"));
        return false;
    }

    const QString base_url = "https://danjuanapp.com/djapi/fund/derived/%1";
    QVariantMap result = get_json_from_networker(base_url.arg(code));
    if (result.empty()) {
        return false;
    }
    result = result["data"].toMap();
    nav = result["unit_nav"].toDouble();
    nav_time = result["end_date"].toString();
    nav_gains = result["nav_grtd"].toDouble();
    return true;
}

bool FundInfo::refresh_fund_info() {
    return get_name_and_valuation_info() && get_nav_info();
}

bool FundInfo::calculate_other_info() {
    if (code.isEmpty()) {
        return false;
    }
    holding_total_cost = holding_unit_cost * holding_share;
    holding_earnings = (nav - holding_unit_cost) * holding_share;
    if (holding_total_cost != 0) {
        holding_earning_rate = holding_earnings / holding_total_cost * 100;
    }
    holding_amount = holding_total_cost + holding_earnings;
    expected_earnings = (valuation - nav) * holding_share;
    return true;
}

QSqlRecord FundInfo::save_to_record() {
    QSqlRecord record = model->record();
    record.setGenerated("id", false);
    record.setValue("code", code);
    record.setValue("name", name);
    record.setValue("holding_unit_cost", holding_unit_cost);
    record.setValue("holding_share", holding_share);
    record.setValue("holding_total_cost", holding_total_cost);
    record.setValue("holding_earnings", holding_earnings);
    record.setValue("holding_earning_rate", holding_earning_rate);
    record.setValue("holding_amount", holding_amount);
    record.setValue("nav", nav);
    record.setValue("nav_time", nav_time);
    record.setValue("nav_gains", nav_gains);
    record.setValue("valuation", valuation);
    record.setValue("valuation_gains", valuation_gains);
    record.setValue("valuation_time", valuation_time);
    record.setValue("expected_earnings", expected_earnings);
    record.setValue("remarks", remarks);
    return record;
}

bool FundInfo::save_to_record(QSqlRecord &record) {
    record.setGenerated("id", false);
    record.setValue("code", code);
    record.setValue("name", name);
    record.setValue("holding_unit_cost", holding_unit_cost);
    record.setValue("holding_share", holding_share);
    record.setValue("holding_total_cost", holding_total_cost);
    record.setValue("holding_earnings", holding_earnings);
    record.setValue("holding_earning_rate", holding_earning_rate);
    record.setValue("holding_amount", holding_amount);
    record.setValue("nav", nav);
    record.setValue("nav_time", nav_time);
    record.setValue("nav_gains", nav_gains);
    record.setValue("valuation", valuation);
    record.setValue("valuation_gains", valuation_gains);
    record.setValue("valuation_time", valuation_time);
    record.setValue("expected_earnings", expected_earnings);
    record.setValue("remarks", remarks);
    return true;
}

void FundInfo::set_info_from_record(const QSqlRecord &record) {
    code = record.value("code").toString();
    name = record.value("name").toString();
    holding_unit_cost = record.value("holding_unit_cost").toDouble();
    holding_share = record.value("holding_share").toDouble();
    holding_total_cost = record.value("holding_total_cost").toDouble();
    holding_earnings = record.value("holding_earnings").toDouble();
    holding_earning_rate = record.value("holding_earning_rate").toDouble();
    holding_amount = record.value("holding_amount").toDouble();
    nav = record.value("nav").toDouble();
    nav_time = record.value("nav_time").toString();
    nav_gains = record.value("nav_gains").toDouble();
    valuation = record.value("valuation").toDouble();
    valuation_gains = record.value("valuation_gains").toDouble();
    valuation_time = record.value("valuation_time").toString();
    expected_earnings = record.value("expected_earnings").toDouble();
    remarks = record.value("remarks").toString();
}

bool FundInfo::insert_new_record_to_database() {
    refresh_fund_info();
    calculate_other_info();
    QSqlRecord record = save_to_record();
    if (!model->insertRecord(-1, record)) {
        QMessageBox::critical(nullptr, tr("基金新增失败"),
                              model->lastError().text());
        qDebug() << model->lastError();
    }
    return true;
}

bool FundInfo::refresh_and_save_record_changes_to_database(int row) {
    refresh_fund_info();
    calculate_other_info();
    QSqlRecord record = model->record(row);
    save_to_record(record);
    if (!model->setRecord(row, record)) {
        QMessageBox::critical(nullptr, tr("基金更新失败"), (tr("基金%1-%2更新失败!\n错误信息：").arg(row).arg(name) +
                                                      model->lastError().text()));
        qDebug() << model->lastError();
    }
    return false;
}
