#ifndef MAINTAININFOMODEL_H
#define MAINTAININFOMODEL_H

#include <QAbstractTableModel>
#include <QDate>
#include <QSettings>
#include <QTimer>
#include <vector>

class MaintainInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit MaintainInfoModel(QObject *parent = nullptr);

    struct MaintainInfo {
        QString content;        // 保养内容
        int cycle;              // 保养周期（单位：天）
        QDate creationDate;     // 创建时间
        QDate nextMaintenance;  // 下次保养时间
        int remainingDays;      // 剩余天数
    };

    void addMaintainInfo(const QString &content, int cycle);
    void reviseMaintainCycle(int index, int cycle);
    void reviseMaintainContent(int index, const QString& content);
    void removeMaintainInfo(int row);
    void resetCountdown(int row);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setHorizontalHeader(int section, const QVariant &value);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void saveToConfigFile(const QString& fileName);
    void loadFromConfigFile(const QString& fileName);

    void startPeriodicUpdate();

private:

    void updateSingleMaintainInfo(int row);
    void updateAllMaintainInfo();

private:
    std::vector<MaintainInfo> m_data;
    std::vector<QString> m_headerData;
    QTimer *timer;
};

#endif // MAINTAININFOMODEL_H
