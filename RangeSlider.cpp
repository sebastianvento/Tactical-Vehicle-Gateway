#include "RangeSlider.h"

#include <QPainter>
#include <QMouseEvent>

// --- RangeSlider Implementation ---
// Implements custom painting, mouse interaction, and coordinate mapping

// --- Construction ---
RangeSlider::RangeSlider(QWidget *parent) : QWidget(parent) {
    // Enable mouse tracking for responsive handle dragging
    setMouseTracking(true);

    // Default logical range (0–100)
    m_minimumRange = 0;
    m_maximumRange = 100;

    // Default visual preset for the slider handles.
    // Logical range remains 0–100; initial selection is narrowed for usability.
    m_lowerValue = 20;
    m_upperValue = 80;
}

// --- Configuration API ---
void RangeSlider::setRange(int min, int max) {
    m_minimumRange = min;
    m_maximumRange = max;

    // Bounds check: Ensure current handles remain within the new logical range
    m_lowerValue = qBound(m_minimumRange, m_lowerValue, m_maximumRange);
    m_upperValue = qBound(m_minimumRange, m_upperValue, m_maximumRange);

    update(); // Trigger repaint
}

void RangeSlider::setValues(int min, int max) {
    m_lowerValue = min;
    m_upperValue = max;

    // Enforce constraints: Range boundaries and handle cross-over prevention
    m_lowerValue = qBound(m_minimumRange, m_lowerValue, m_maximumRange);
    m_upperValue = qBound(m_lowerValue, m_upperValue, m_maximumRange);

    update();
}

// --- Rendering ---
void RangeSlider::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int yCenter = height() / 2;
    const int xLower = valueToPosition(m_lowerValue);
    const int xUpper = valueToPosition(m_upperValue);
    const int margin = 15;

    // Draw Background Track
    painter.setPen(QPen(Qt::lightGray, 6, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(margin, yCenter, width() - margin, yCenter);

    // Draw Active Range (Highlight)
    painter.setPen(QPen(QColor(0, 120, 215), 6, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(xLower, yCenter, xUpper, yCenter);

    // Draw Interactive Handles
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPoint(xLower, yCenter), 10, 10);
    painter.drawEllipse(QPoint(xUpper, yCenter), 10, 10);
}

// --- Mouse Interaction ---
void RangeSlider::mousePressEvent(QMouseEvent *event) {
    const int mouseX = event->pos().x();
    const int posLower = valueToPosition(m_lowerValue);
    const int posUpper = valueToPosition(m_upperValue);
    const int hitTolerance = 20; // Hitbox radius in pixels

    // Determine active handle (lower handle takes precedence on overlap)
    if (qAbs(mouseX - posLower) < hitTolerance) {
        m_isDraggingLower = true;
        setCursor(Qt::ClosedHandCursor);
    } else if (qAbs(mouseX - posUpper) < hitTolerance) {
        m_isDraggingUpper = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event) {
    const int currentVal = positionToValue(event->pos().x());

    if (m_isDraggingLower) {
        // Prevent lower handle from exceeding upper handle
        m_lowerValue = qBound(m_minimumRange, currentVal, m_upperValue);
        emit valuesChanged(m_lowerValue, m_upperValue);
        update();
    } else if (m_isDraggingUpper) {
        // Prevent upper handle from dropping below lower handle
        m_upperValue = qBound(m_lowerValue, currentVal, m_maximumRange);
        emit valuesChanged(m_lowerValue, m_upperValue);
        update();
    }
}

void RangeSlider::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    m_isDraggingLower = false;
    m_isDraggingUpper = false;
    unsetCursor();
}

// --- Coordinate Mapping ---
int RangeSlider::valueToPosition(int value) {
    const int margin = 20; // Visual padding for handle radius

    if (m_maximumRange <= m_minimumRange || width() <= 2 * margin) {
        return margin;
    }

    const double rangeWidth = m_maximumRange - m_minimumRange;
    const double pixelWidth = width() - (2 * margin);
    const double ratio = (static_cast<double>(value) - m_minimumRange) / rangeWidth;

    return margin + static_cast<int>(ratio * pixelWidth);
}

int RangeSlider::positionToValue(int position) {
    const int margin = 20;

    if (width() <= 2 * margin) {
        return m_minimumRange;
    }

    const double pixelWidth = width() - (2 * margin);
    const double ratio = static_cast<double>(position - margin) / pixelWidth;
    const int value = m_minimumRange + static_cast<int>(ratio * (m_maximumRange - m_minimumRange));

    return qBound(m_minimumRange, value, m_maximumRange);
}
