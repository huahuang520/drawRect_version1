#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QGraphicsItem>
#include <QStandardPaths>
#include <QPixmap>
#include <QContextMenuEvent>
#include <QMenu>
#include <QFileDialog>
#include <QDebug>
#include "DrawROI.h"
#include "GraphicsViewTool.h"
//#include "DrawPolygons.h"

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphicsView(QWidget *parent);
	~GraphicsView();

public:
	QImage GetImage();
    void SetPix(QPixmap pix);

    qreal GetScaledValue();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* ev);
    void contextMenuEvent(QContextMenuEvent* ev); //右击菜单

    void rotate();  //test

// ----------------ROI operation----------------------
public:
    void setBackImage(QImage img);   //设置背景图片  外部调用

   // void drawRect1();
private:
    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_PixmapItem;
	QImage m_image;
    QPixmap pix;
    QPoint lasetPoint;
    QPoint endPoint;
    QPixmap tempPix;
    qreal m_scaledValue;  //缩放倍数
    QMenu* m_pOptMenu;
    QAction* m_pDelAction;
    QAction* m_pSaveAction;
    bool isDrawROI;

    DrawROI* m_drawROI;
    GraphicsViewTool* m_graphicsViewTool;

   // DrawControl* drawControl;
};
