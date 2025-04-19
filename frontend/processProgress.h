#ifndef PROCESS_PROGRESS_H
#define PROCESS_PROGRESS_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QPainter>

/**
 * @brief A custom delegate for rendering progress bars in QTreeView widgets
 *
 * This delegate renders progress values (0-100) as visual progress bars
 * in tree views. Values outside the valid range will be displayed normally.
 */
class ProgressBarDelegate : public QStyledItemDelegate
{
public:
    /**
     * @brief Constructor that inherits from QStyledItemDelegate
     * @param parent The parent object
     */
    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * @brief Custom paint method to render progress bars
     * @param painter The painter to use for rendering
     * @param option Style options for the item
     * @param index Model index containing the data to be rendered
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;
};

#endif // PROCESS_PROGRESS_H