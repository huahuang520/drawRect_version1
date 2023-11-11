#pragma once

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMenu>
#include <QFileDialog>
#include <QStandardPaths>
#include <QWheelEvent>

#define EDGE_WIDTH       2       //�߿�Ŀ��
#define MIDDLELINE_WIDTH 1       //�����ߵĿ��

#define POINT_WIDTH      6        //��Ե9��Ŀ��
#define POINT_HEIGHT     6        //��Ե9��ĸ߶�

#define EDGPADDING       5        //���ܱ�Ե��������
#define CORPADDING       6        //�Ľǿ�������

#define MIN_WIDTH        5        //���������С���
#define MIN_HEIGHT       5        //���������С�߶�

/* ���ŷ��� */
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
    void drawROI(); // ����QGraphicsView paintEvent��ʵʱ����ROI����    -----------����
    void changeROIBegin(const QPoint& point);  //����QGraphicsView mousePressEvent�У�pointΪ��ͼ���꣬���տ�ʼ���¸�����������ROI���滹���������ж��Ƿ��ƶ�roi���ǻ��µ�roi   --------����
    void changeROIInProgress(const QPoint& point);  //����QGraphicsView mouseMoveEvent�У�pointΪ��ͼ���꣬��갴���ƶ���ʱ���ROI���»��ƻ����Ż��ƶ�   ---------����
    void changeROIEnd(const QPoint& point); //����QGraphicsView mouseReleaseEvent�У�pointΪ��ͼ���꣬����ͷŵ�ʱ�����ROI״̬�ı�   ---------����
    void changeMouseShape(const QPoint& point); //����QGraphicsView mouseMoveEvent��, pointΪ��ͼ���꣬��������λ�����õ�ǰ�������״   ----------����
    std::map<int, QRect> getRoiRect() const; //��������roi��map
    void DeleteCurrentROI(); //ɾ����ǰroi
    void DeleteAllROIS(); //ɾ������roi
    void saveAllROIImage(); //����roiͼ��
    void saveCurrentROIImage(); //���浱ǰroiͼ��
    //void SetBackImage(QImage* img);
    void SetBackPix(QPixmap& pix);  //����resizeEvent����ȡʵʱpix��С   ----------����
    void isDrawMultipleROIs(bool isMultioROIS); //�����Ƿ�������ƶ��roi��Ĭ��ֻ�ܻ���һ��
    void ScaleROIS(QWheelEvent* event, double scale); //����roi, ����wheelEvent��������Ϊ��������

    /*  �����ڲ��һ��˵�
    void GraphicsView::contextMenuEvent(QContextMenuEvent* ev)
    {
        QPoint mousePos = ev->pos();

        //QRect.contains�����ĵ��ھ����ڲ�����true
        //��������
        QPointF fScenePoint = this->mapToScene(mousePos);
        //ͼ��������
        QGraphicsItem* pItem = this->scene()->itemAt(fScenePoint, QTransform());
        if (NULL != pItem)
        {
            QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
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
    void initViewer();                         //��ʼ��
    EmDirection region(const QPoint& point); //�ж���굱ǰ������ROIʲôλ��
    void paintRect(const QPoint& mousePoint);  //���ƾ���
    void moveRect(const QPoint& mousePoint);   //�ƶ�����
    void scaleRect(const QPoint& mousePoint);  //���ž���
private:
    QGraphicsView* m_graphicsView;
    bool isBeginGraw;
    bool isROIBeginChange; //��ֹ���roi�����ʱ�����
    QImage m_image;  //����ͼ��
    QPixmap m_pixmap; //��imageת����pix
    bool m_bPainterPressed;        //�Ƿ����ڻ���
    bool m_bMovedPressed;          //�Ƿ������϶�
    bool m_bScalePressed;          //�Ƿ��������Ŵ�С
    QPoint m_paintStartPoint;      //���Ƶĳ�ʼλ��
    QPoint m_moveStartPoint;       //�϶��ĳ�ʼλ��
    EmDirection m_emCurDir;        //�϶��ķ���

    std::map<int, QRect> m_rois; //���ƶ��roi
    bool m_isDrawMultipROIS;
    int m_currentROIIndex;
    EmDirection m_currentDir;

    std::map<int, QRectF> m_OriginRois; //ÿ��changeROIEnd�������ᱣ��һ��roiԭʼ����,�����ԭʼ���ݲ�����������
};
