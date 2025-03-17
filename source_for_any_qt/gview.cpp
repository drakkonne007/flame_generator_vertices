#include "gview.h"
#include <QScrollBar>
#include <QWheelEvent>

GView::GView()
{
    setMouseTracking(true);
}

void GView::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0){
        scale(1.1,1.1);
    }else{
        scale(0.9,0.9);
    }
}

void GView::clearInternalData()
{
    m_coordLines.clear();
    m_points.clear();
    m_anchors = nullptr;
    m_underMouseCircle = nullptr;
    m_tempRectLines.clear();
}

void GView::mousePressEvent(QMouseEvent *event)
{
    if(!m_isLoadImg){
        return;
    }
    if(event->button() == Qt::LeftButton){
        m_leftButton = true;
        for(auto item : m_points){
            if(item->contains(mapToScene(event->pos()))){
                m_underMouseCircle = item;
                return;
            }
        }
        m_points<<scene()->addEllipse(mapToScene(event->pos()).x() - m_anchSize/2
                                     ,mapToScene(event->pos()).y() - m_anchSize/2,m_anchSize,m_anchSize,QPen(Qt::blue),QBrush(Qt::blue));
        m_points.last()->setZValue(10000);
        refreshTempLines();
    }else if(event->button() == Qt::RightButton){
        if(m_anchors){
            delete m_anchors;
        }
        m_anchors = scene()->addEllipse(mapToScene(event->pos()).x() - m_anchSize/2
                                        ,mapToScene(event->pos()).y() - m_anchSize/2,m_anchSize,m_anchSize,QPen(Qt::red),QBrush(Qt::red));
        m_anchors->setZValue(9000);
        emit createAnchor(QPoint(mapToScene(event->pos()).x(),mapToScene(event->pos()).y()));
        m_rightButton = true;
    }else if(event->button() == Qt::MiddleButton){
        m_midButton = true;
    }
}

void GView::changeAnchor(const QPointF &point)
{
    if(m_anchors){
        delete m_anchors;
    }
    m_anchors = scene()->addEllipse(point.x(),point.y(),m_anchSize,m_anchSize,QPen(Qt::red),QBrush(Qt::red));
    m_anchors->setZValue(9000);
}

void GView::refreshTempLines()
{
    for(auto item : m_tempRectLines){
        delete item;
    }
    m_tempRectLines.clear();
    QPen pen(QBrush(QColor(Qt::blue)),1,Qt::DashLine);
    for(int i = 0;i<m_points.size() - 1;i++){
        m_tempRectLines << scene()->addLine(m_points[i]->rect().center().x(),m_points[i]->rect().center().y()
                                           ,m_points[i+1]->rect().center().x(),m_points[i+1]->rect().center().y(),pen);
    }
}

void GView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton){
        m_midButton = false;
    }else if(event->button() == Qt::RightButton){
        m_rightButton = false;
    }else if(event->button() == Qt::LeftButton){
        m_leftButton = false;
    }
}

void GView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_midButton){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->x() + m_prevPos.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - event->y() + m_prevPos.y());
    }else if(m_rightButton){
        if(m_anchors){
            delete m_anchors;
        }
        m_anchors = scene()->addEllipse(mapToScene(event->pos()).x() - m_anchSize/2
                                        ,mapToScene(event->pos()).y() - m_anchSize/2,m_anchSize,m_anchSize,QPen(Qt::red),QBrush(Qt::red));
        m_anchors->setZValue(9000);
        emit createAnchor(QPoint(mapToScene(event->pos()).x(),mapToScene(event->pos()).y()));
    }else if(m_leftButton && m_underMouseCircle){
        m_underMouseCircle->setRect(mapToScene(event->pos()).x() - m_anchSize/2
                                    ,mapToScene(event->pos()).y() - m_anchSize/2,m_anchSize,m_anchSize);
        refreshTempLines();
    }
    m_prevPos = event->pos();
}
