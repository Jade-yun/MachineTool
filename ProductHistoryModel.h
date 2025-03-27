#ifndef PRODUCTHISTORYMODEL_H
#define PRODUCTHISTORYMODEL_H

#include <QAbstractTableModel>
#include <QList>

class ProductHistoryModel : public QAbstractTableModel
{
    Q_OBJECT


public:
    explicit ProductHistoryModel(QObject *parent = nullptr);

    struct ProductInfo {
        QString time;           // 时间
        int productNum;         // 产量
        double formingCycle;    // 成型周期
        double fetchTime;       // 取物时间
    };
    // 必须重写的方法
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 添加新记录
    void appendProductRecord(int actualProductNum, double formingCycle, double fetchTime);
    /**
     * @brief 清除所有产品信息记录
     */
    void clearAllProductRecord();

private:
    void saveToSettings();
    void loadFromSettings();
private:
    QList<ProductInfo> m_data;
    const int MAX_ROWS = 50;  // 最大记录数
};

#endif // PRODUCTTABLEMODEL_H
