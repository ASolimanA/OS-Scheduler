#ifndef TOGGLE_H
#define TOGGLE_H

#include <QWidget>
#include <QPropertyAnimation>

class ToggleSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
public:
    explicit ToggleSwitch(QWidget *parent = nullptr);
    bool isChecked() const;
    void setChecked(bool checked);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_checked = false;
    QPropertyAnimation *m_thumbAnimation;
    QColor m_thumbColor = Qt::white;
    QColor m_trackColorChecked = QColor("#4cd964");
    QColor m_trackColorUnchecked = QColor("#3A59D1");
};

#endif // TOGGLE_H
