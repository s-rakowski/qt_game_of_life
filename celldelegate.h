#include <qabstractitemdelegate.h>
#include <qcolor.h>

#ifndef CELLDELEGATE_H
#define CELLDELEGATE_H


class CellDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    CellDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
public slots:
    void setCellSize(int size);
    void setBorderSize(int size);
    void setActiveColor(const QColor &col);
    void setInactiveColor(const QColor &col);
    void setBackgroundColor(const QColor &col);
private:
    int cellSize;
    int borderSize;
    QColor activeColor;
    QColor inactiveColor;
    QColor bgColor;
};

#endif // CELLDELEGATE_H
