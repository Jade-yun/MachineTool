#include "maintaininfomodel.h"

// 构造函数
MaintainInfoModel::MaintainInfoModel(QObject *parent) : QAbstractTableModel(parent)
  , m_headerData(4, QString())
{
    timer = new QTimer(this);
}

// 必须实现的虚函数
int MaintainInfoModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_data.size();
}

int MaintainInfoModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 4; // 列数：内容、周期、剩余天数、下次保养时间
}

QVariant MaintainInfoModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto &info = m_data.at(index.row());
    switch (index.column()) {
    case 0: return info.content;
    case 1: return info.cycle;
    case 2: return QDate::currentDate().daysTo(info.nextMaintenance);
    case 3: return info.nextMaintenance.toString("yyyy-MM-dd");
    default: return {};
    }
}

QVariant MaintainInfoModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return {};  // 只处理 DisplayRole

    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < m_headerData.size()) {
            return m_headerData.at(section);
        }
    }
    else if (orientation == Qt::Vertical) {
        return QString::number(section + 1);
    }
    return {};
}


// 添加保养信息
void MaintainInfoModel::addMaintainInfo(const QString &content, int cycle) {
    QDate currentDate = QDate::currentDate();
    QDate nextMaintenance = currentDate.addDays(cycle);

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back({content, cycle, currentDate, nextMaintenance, cycle});
    endInsertRows();
}

void MaintainInfoModel::reviseMaintainCycle(int index, int cycle)
{
    if (index < m_data.size() && index > 0)
    {
        auto &info = m_data[index];
        info.cycle = cycle;
        info.nextMaintenance = info.creationDate.addDays(info.cycle);
        info.remainingDays = QDate::currentDate().daysTo(info.nextMaintenance);
    }
}

void MaintainInfoModel::reviseMaintainContent(int index, const QString &content)
{
    if (index < m_data.size() && index > 0)
    {
        auto &info = m_data[index];
        info.content = content;
    }
}

// 移除保养信息
void MaintainInfoModel::removeMaintainInfo(int row) {
    if (row < 0 || row >= m_data.size())
        return;
    beginRemoveRows(QModelIndex(), row, row);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();
}

// 重置倒计时
void MaintainInfoModel::resetCountdown(int row) {
    if (row < 0 || row >= m_data.size())
        return;

    auto &info = m_data[row];
    info.creationDate = QDate::currentDate();
    info.nextMaintenance = info.creationDate.addDays(info.cycle);
    info.remainingDays = info.cycle;

    emit dataChanged(index(row, 2), index(row, 3));
}

bool MaintainInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)

    if (!checkIndex(index))
        return false;
    auto &info = m_data[index.row()];
    if (index.column() == 0)
    {
        info.content = value.toString();
    }
    else if (index.column() == 1)
    {
        info.cycle = value.toInt();
        info.nextMaintenance = info.creationDate.addDays(info.cycle);
        info.remainingDays = QDate::currentDate().daysTo(info.nextMaintenance);
    }
    return true;
}

void MaintainInfoModel::setHorizontalHeader(int section, const QVariant &value)
{
    if (section >= 0 && section < m_headerData.size()) {
        m_headerData[section] = value.toString();
        emit headerDataChanged(Qt::Horizontal, section, section);
    }
}

// 保存数据
void MaintainInfoModel::saveToConfigFile(const QString& fileName){
    QSettings settings(fileName, QSettings::IniFormat);
    settings.setIniCodec("utf-8");

    settings.remove("");
    settings.beginWriteArray("maintenanceData");

    for (int row = 0; row < m_data.size(); ++row) {
        settings.setArrayIndex(row);

        const MaintainInfo &info = m_data[row];
        settings.setValue("content", info.content);
        settings.setValue("cycle", info.cycle);
        settings.setValue("creationDate", info.creationDate.toString("yyyy-MM-dd HH:mm:ss"));
        settings.setValue("nextMaintenance", info.nextMaintenance.toString("yyyy-MM-dd HH:mm:ss"));
    }
    settings.endArray();
}

// 加载数据
void MaintainInfoModel::loadFromConfigFile(const QString& fileName) {
    QSettings settings(fileName, QSettings::IniFormat);
    settings.setIniCodec("utf-8");

    beginResetModel();
    m_data.clear();
    int rowCount = settings.beginReadArray("maintenanceData");
    for (int i = 0; i < rowCount; ++i) {
        settings.setArrayIndex(i);
        QString content = settings.value("content").toString();
        int cycle = settings.value("cycle").toInt();
        QDate creationDate = QDate::fromString(settings.value("creationDate").toString(), "yyyy-MM-dd HH:mm:ss");
        QDate nextMaintenance = QDate::fromString(settings.value("nextMaintenance").toString(), "yyyy-MM-dd HH:mm:ss");
        int remainingDays = QDate::currentDate().daysTo(nextMaintenance);

        m_data.push_back({content, cycle, creationDate, nextMaintenance, remainingDays});
    }
    settings.endArray();
    endResetModel();
}

void MaintainInfoModel::updateSingleMaintainInfo(int row) {
    QDate currentDate = QDate::currentDate();
    MaintainInfo &info = m_data[row];

    int daysPassed = info.creationDate.daysTo(currentDate);
    if (daysPassed >= 1) {
        info.remainingDays -= daysPassed;
        if (info.remainingDays < 0) {
            info.remainingDays += info.cycle;
            info.nextMaintenance = currentDate.addDays(info.remainingDays);
        }
        info.creationDate = currentDate;
        emit dataChanged(index(row, 0), index(row, 3));
    }
}

// 更新所有保养信息
void MaintainInfoModel::updateAllMaintainInfo() {
    for (int row = 0; row < m_data.size(); ++row) {
        updateSingleMaintainInfo(row);
    }
}

// 周期性更新
void MaintainInfoModel::startPeriodicUpdate() {
    connect(timer, &QTimer::timeout, this, &MaintainInfoModel::updateAllMaintainInfo);
    timer->start(60 * 60 * 1000); // 每小时触发一次
}
