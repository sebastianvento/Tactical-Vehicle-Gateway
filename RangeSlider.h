#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QWidget>

/**
 * @class RangeSlider
 * @brief Dual-handle range selector widget.
 *
 * Provides an interactive control for selecting a minimum and maximum
 * value within a bounded numeric range.
 */
class RangeSlider : public QWidget {
    Q_OBJECT

public:
    explicit RangeSlider(QWidget *parent = nullptr);

    // --- Configuration ---
    void setRange(int min, int max);
    void setValues(int min, int max);

    // --- Value Access ---
    int lowerValue() const { return m_lowerValue; }
    int upperValue() const { return m_upperValue; }

signals:
    /**
     * @brief Emitted when either handle position changes.
     * @param min Current lower bound value
     * @param max Current upper bound value
     */
    void valuesChanged(int min, int max);

protected:
    // --- Rendering & Interaction ---
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // --- Range State ---
    int m_minimumRange = 0;
    int m_maximumRange = 100;
    int m_lowerValue   = 0;
    int m_upperValue   = 100;

    // --- Interaction State ---
    bool m_isDraggingLower = false;
    bool m_isDraggingUpper = false;

    // --- Coordinate Mapping ---
    int valueToPosition(int value);
    int positionToValue(int position);
};

#endif // RANGESLIDER_H
