#include "processProgress.h"
#include <QApplication>
#include <QStyleOptionProgressBar>

void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    // Get the progress value (should be 0-100)
    float progress = index.data(Qt::DisplayRole).toFloat();

    if (progress >= 0 && progress <= 100)
    {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4); // Add some padding
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;
        progressBarOption.text = QString("%1%").arg(QString::number(progress, 'f', 2));
        progressBarOption.textVisible = true;

        // Draw the progress bar using the application style
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);
    }
    else
    {
        // For non-progress values, use default painting
        QStyledItemDelegate::paint(painter, option, index);
    }
}
bool ProgressBarDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    return false;
}
QWidget *ProgressBarDelegate::createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
{
    return Q_NULLPTR;
}