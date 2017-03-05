#include "imagemodel.h"

ImageModel::ImageModel(QObject *parent)
{

}

void ImageModel::setImage(const QImage &image, bool update)
{
    if(update)
        beginResetModel();

    modelImage = image;

    if(update)
        endResetModel();
}

QImage ImageModel::image()
{
    return modelImage;
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    return modelImage.height();
}

int ImageModel::columnCount(const QModelIndex &parent) const
{
    return modelImage.width();
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    else
    {
        return QVariant(modelImage.pixelIndex(index.column(), index.row()));
    }
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::SizeHintRole)
        return QSize(1, 1);
    return QVariant();
}

bool ImageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || role != Qt::EditRole)
        return false;
    else
    {
        if(value.toInt() == 0)
            modelImage.setPixel(index.column(), index.row(), 0);
        else
            modelImage.setPixel(index.column(), index.row(), 1);
        dataChanged(index, index);
        return true;
    }
}
