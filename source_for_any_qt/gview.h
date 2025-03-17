#pragma once
#include <QGraphicsLineItem>
#include <QGraphicsView>

class GView : public QGraphicsView
{
    Q_OBJECT

signals:
    void createAnchor(const QPoint &size);

public:
    GView();
    QVector<QGraphicsLineItem*> m_coordLines;
    QVector<QGraphicsEllipseItem*> m_points;
    bool m_isLoadImg = false;
    void clearInternalData();
    QVector<QGraphicsLineItem*> m_tempRectLines;
    void refreshTempLines();
    QGraphicsEllipseItem* m_underMouseCircle = nullptr;

    void changeAnchor(const QPointF &point);
private:
    QGraphicsEllipseItem * m_anchors = nullptr;
    bool m_midButton = false;
    bool m_rightButton = false;
    bool m_leftButton = false;
    QPoint m_prevPos;
    double m_anchSize = 1;

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};
