#include "toggle.h"
#include <QPainter>
#include <QMouseEvent>

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QWidget(parent)
{
    // Set the fixed size for the toggle switch
    setFixedSize(60, 30);

    // Setup animation for smooth toggle effect
    m_thumbAnimation = new QPropertyAnimation(this, "checked");
    m_thumbAnimation->setDuration(150); // 150ms animation
}

bool ToggleSwitch::isChecked() const
{
    return m_checked;
}

void ToggleSwitch::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

    // Update the widget
    update();

    // Emit signal
    emit toggled(m_checked);
}

void ToggleSwitch::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Calculate sizes and positions
    int width = this->width();
    int height = this->height();

    // Track/background rectangle
    QRect track(0, 0, width, height);

    // Thumb/handle size
    int thumbSize = height - 4;
    int thumbPos = m_checked ? width - thumbSize - 2 : 2;
    QRect thumb(thumbPos, 2, thumbSize, thumbSize);

    // Draw track/background
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_checked ? m_trackColorChecked : m_trackColorUnchecked);
    painter.drawRoundedRect(track, height / 2, height / 2);

    // Draw thumb/handle
    painter.setBrush(m_thumbColor);
    painter.drawEllipse(thumb);
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setChecked(!m_checked);
    }

    QWidget::mousePressEvent(event);
}