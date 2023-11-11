#pragma once

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMenu>
#include <QFileDialog>
#include <QStandardPaths>
#include <QWheelEvent>

#define EDGE_WIDTH       2       //边框的宽度
#define MIDDLELINE_WIDTH 1       //辅助线的宽度

#define POINT_WIDTH      6        //边缘9点的宽度
#define POINT_HEIGHT     6        //边缘9点的高度

#define EDGPADDING       5        //四周边缘可拉伸宽度
#define CORPADDING       6        //四角可拉伸宽度

#define MIN_WIDTH        5        //可拉伸的最小宽度
#define MIN_HEIGHT       5        //可拉伸的最小高度

/* 缩放方向 */
enum EmDirection
{
    DIR_TOP = 0,
    DIR_BOTTOM,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_LEFTTOP,
    DIR_LEFTBOTTOM,
    DIR_RIGHTTOP,
    DIR_RIGHTBOTTOM,
    DIR_MIDDLE,
    DIR_NONE
};

class DrawROI : public QObject
{
	Q_OBJECT

public:
	DrawROI(QGraphicsView* GraphicsView, QObject*parent = nullptr);
	~DrawROI();
public:
    void drawROI(); // 放在QGraphicsView paintEvent中实时更新ROI样子    -----------必须
    void changeROIBegin(const QPoint& point);  //放在QGraphicsView mousePressEvent中，point为视图坐标，鼠标刚开始点下根据坐标是在ROI里面还是外面来判断是否移动roi还是画新的roi   --------必须
    void changeROIInProgress(const QPoint& point);  //放在QGraphicsView mouseMoveEvent中，point为视图坐标，鼠标按下移动的时候对ROI重新绘制或缩放或移动   ---------必须
    void changeROIEnd(const QPoint& point); //放在QGraphicsView mouseReleaseEvent中，point为视图坐标，鼠标释放的时候结束ROI状态改变   ---------必须
    void changeMouseShape(const QPoint& point); //放在QGraphicsView mouseMoveEvent中, point为视图坐标，根据鼠标的位置设置当前的鼠标形状   ----------必须
    std::map<int, QRect> getRoiRect() const; //返回所有roi的map
    void DeleteCurrentROI(); //删除当前roi
    void DeleteAllROIS(); //删除所有roi
    void saveAllROIImage(); //保存roi图像
    void saveCurrentROIImage(); //保存当前roi图像
    //void SetBackImage(QImage* img);
    void SetBackPix(QPixmap& pix);  //放在resizeEvent，获取实时pix大小   ----------必须
    void isDrawMultipleROIs(bool isMultioROIS); //设置是否允许绘制多个roi，默认只能绘制一个
    void ScaleROIS(QWheelEvent* event, double scale); //缩放roi, 放在wheelEvent，参数二为缩放因子

    /*  矩形内部右击菜单
    void GraphicsView::contextMenuEvent(QContextMenuEvent* ev)
    {
        QPoint mousePos = ev->pos();

        //QRect.contains被给的点在矩形内部返回true
        //场景坐标
        QPointF fScenePoint = this->mapToScene(mousePos);
        //图形项坐标
        QGraphicsItem* pItem = this->scene()->itemAt(fScenePoint, QTransform());
        if (NULL != pItem)
        {
            QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
            QPoint itemPoint(fItemPoint.x(), fItemPoint.y());
            if (m_drawROI->getRoiRect().contains(itemPoint))
            {
                m_pOptMenu->exec(QCursor::pos());
            }
        }
        ev->accept();
        }
    */

private:
    void initViewer();                         //初始化
    EmDirection region(const QPoint& point); //判断鼠标当前坐标在ROI什么位置
    void paintRect(const QPoint& mousePoint);  //绘制矩形
    void moveRect(const QPoint& mousePoint);   //移动矩形
    void scaleRect(const QPoint& mousePoint);  //缩放矩形
private:
    QGraphicsView* m_graphicsView;
    bool isBeginGraw;
    bool isROIBeginChange; //防止多个roi交会的时候出错
    QImage m_image;  //背景图像
    QPixmap m_pixmap; //由image转化的pix
    bool m_bPainterPressed;        //是否正在绘制
    bool m_bMovedPressed;          //是否正在拖动
    bool m_bScalePressed;          //是否正在缩放大小
    QPoint m_paintStartPoint;      //绘制的初始位置
    QPoint m_moveStartPoint;       //拖动的初始位置
    EmDirection m_emCurDir;        //拖动的方向

    std::map<int, QRect> m_rois; //绘制多个roi
    bool m_isDrawMultipROIS;
    int m_currentROIIndex;
    EmDirection m_currentDir;

    std::map<int, QRectF> m_OriginRois; //每次changeROIEnd结束都会保存一份roi原始数据,方便对原始数据操作比如缩放
};
