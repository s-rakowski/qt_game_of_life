#include <QImage>
#include <QAbstractTableModel>

#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

class ImageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ImageModel(QObject *parent = 0);
    void setImage(const QImage &image, bool update = true);
    QImage image();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
public slots:
    bool setData(const QModelIndex &index, const QVariant &value, int role);
private:
    QImage modelImage;
};

#endif // IMAGEMODEL_H
