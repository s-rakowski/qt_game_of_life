#include "celldelegate.h"
#include "qpainter.h"

CellDelegate::CellDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
    cellSize = 8;
    borderSize = 0;
    activeColor = QColor(QString("black"));
    inactiveColor = QColor(QString("white"));
    bgColor = QColor(QString("white"));
}

void CellDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(borderSize != 0)
        painter->fillRect(option.rect, bgColor);

    painter->save();
    if(index.model()->data(index).toInt() == 0)
        painter->setBrush(inactiveColor);
    else
        painter->setBrush(activeColor);

    painter->setPen(Qt::NoPen);
    QRect r = QRect(0, 0, cellSize, cellSize);
    r.moveCenter(option.rect.center());
    painter->drawRect(r);

    painter->restore();
}

QSize CellDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(cellSize + borderSize, cellSize + borderSize);
}

void CellDelegate::setCellSize(int size)
{
    cellSize = size;
}

void CellDelegate::setBorderSize(int size)
{
    borderSize = size;
}

void CellDelegate::setActiveColor(const QColor &col)
{
    activeColor = col;
}

void CellDelegate::setInactiveColor(const QColor &col)
{
    inactiveColor = col;
}

void CellDelegate::setBackgroundColor(const QColor &col)
{
   bgColor = col;
}
