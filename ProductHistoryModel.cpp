#include "ProductHistoryModel.h"

#include <QVariant>
#include <QDateTime>
#include <QSettings>

const QString ProductHistroyInfoPath = "/opt/MachineTool/configs/ProductHistroyInfo.ini";

ProductHistoryModel::ProductHistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    loadFromSettings();
}

int ProductHistoryModel::rowCount(const QModelIndex &) const
{
    return m_data.size();
}

int ProductHistoryModel::columnCount(const QModelIndex &) const
{
    return 4; // 四列：时间、产量、成型周期、取物时间
}

QVariant ProductHistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const auto& info = m_data[index.row()];

    switch (index.column()) {
    case 0: return info.time;
    case 1: return info.productNum;
    case 2: return info.formingCycle;
    case 3: return info.fetchTime;
    default: return {};
    }
}

QVariant ProductHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return QString(tr("时间"));
        case 1: return QString(tr("产量"));
        case 2: return QString(tr("成型周期 (s)"));
        case 3: return QString(tr("取物时间 (s)"));
        }
    }
    else if (orientation == Qt::Vertical)
    {
        return QString::number(section + 1);
    }
    return QVariant();
}

void ProductHistoryModel::appendProductRecord(int actualProductNum, double formingCycle, double fetchTime)
{
    beginInsertRows(QModelIndex(), 0, 0);

    // 追加新数据
    m_data.prepend({ QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                    actualProductNum,
                    formingCycle,
                    fetchTime });

    // 限制最大存储条数
    if (m_data.size() > MAX_ROWS)
        m_data.removeLast();

    endInsertRows();

    // 自动保存到 QSettings
    saveToSettings();
}

void ProductHistoryModel::clearAllProductRecord()
{
    m_data.clear();
}

void ProductHistoryModel::saveToSettings()
{
    QSettings settings(ProductHistroyInfoPath, QSettings::IniFormat);
    settings.setIniCodec("utf-8");
    settings.remove("");

    settings.setValue("RowCount", m_data.size());

    for (int i = 0; i < m_data.size(); ++i) {
        settings.setValue(QString("row%1/time").arg(i), m_data[i].time);
        settings.setValue(QString("row%1/productNum").arg(i), m_data[i].productNum);
        settings.setValue(QString("row%1/formingCycle").arg(i), m_data[i].formingCycle);
        settings.setValue(QString("row%1/fetchTime").arg(i), m_data[i].fetchTime);
    }

    settings.endGroup();

    settings.sync();
}

void ProductHistoryModel::loadFromSettings()
{
    QSettings settings(ProductHistroyInfoPath, QSettings::IniFormat);
    settings.setIniCodec("utf-8");

    int rowCount = settings.value("RowCount", 0).toInt();
    beginResetModel();
    m_data.clear();

    for (int i = 0; i < rowCount; ++i) {
        ProductInfo info;
        info.time = settings.value(QString("row%1/time").arg(i)).toString();
        info.productNum = settings.value(QString("row%1/productNum").arg(i)).toInt();
        info.formingCycle = settings.value(QString("row%1/formingCycle").arg(i)).toDouble();
        info.fetchTime = settings.value(QString("row%1/fetchTime").arg(i)).toDouble();
        m_data.prepend(info);
    }

    settings.endGroup();
    endResetModel();
}
