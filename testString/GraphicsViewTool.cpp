#include "GraphicsViewTool.h"

GraphicsViewTool::GraphicsViewTool(QObject *parent)
	: QObject(parent)
{

}

GraphicsViewTool::GraphicsViewTool(QGraphicsView* graphicsView, QGraphicsScene* scene, QGraphicsPixmapItem* pixmapItem, QObject* parent)
    : QObject(parent)
{
    m_graphicsView = graphicsView;
    m_scene = scene;
    m_pixmapItem = pixmapItem;
	m_scale = 1;
}

GraphicsViewTool::~GraphicsViewTool()
{
}

void GraphicsViewTool::SetPixMiddle(const QImage& image)
{
    QSize s = m_graphicsView->size();
    //��ͼ������汣���ݺ�ȣ�ͼ����Խ������С��һ����Ϊpix�Ŀ��ߣ��������仯��
	//������Ϊthis->width(), this->height()������this->viewport()->width()��this->viewport()->height()������Ҫ�ܶ������֤
    QPixmap pix = QPixmap::fromImage(image).scaled(s, Qt::KeepAspectRatio);  //����仯����ı�image��size����Ϊ�Ȱ�image�����pix��Ȼ���pix�������ţ�����ֻ�ı���Pix��size
    m_pixmap = pix;
	int w = pix.width();
	int h = pix.height();

    m_pixmapItem->setPixmap(pix);

    m_scene->setSceneRect(0, 0, pix.width(), pix.height());  //����ͼ����scene������

    m_graphicsView->viewport()->update();
}

QPixmap& GraphicsViewTool::GetCurrentPix()
{
    return m_pixmap;
}

void GraphicsViewTool::ScaleImage(QWheelEvent* event)
{
    if (Qt::CTRL == event->modifiers())   //����CTRL��
    {
        // ��ȡ��ǰ��������view��λ��;
        QPointF cursorPoint = event->pos();
        // ��ȡ��ǰ��������scene��λ��;
        QPointF scenePos = m_graphicsView->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
		//ͼ��������
		QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(scenePos, QTransform());

		if (NULL != pItem)
		{
			QPointF fItemPoint = pItem->mapFromScene(scenePos);   //��ȡͼ��������

			int a = 0;
		}


		// ��ȡview�Ŀ��;
		qreal viewWidth = m_pixmap.width();
		qreal viewHeight = m_pixmap.height();

		// ��ȡ��ǰ���λ���൱��view��С�ĺ��ݱ���;
		qreal hScale = cursorPoint.x() / viewWidth;
		qreal vScale = cursorPoint.y() / viewHeight;

		// ��ǰ��������;
		qreal scaleFactor = m_graphicsView->matrix().m11(); //m11Ϊˮƽ����
		int wheelDeltaValue = event->delta();
		// ���Ϲ������Ŵ�;
		if (wheelDeltaValue > 0)
		{
			m_graphicsView->scale(1.2, 1.2);
			m_scale = 1.2;
		}
		// ���¹�������С;
		else
		{
			m_graphicsView->scale(1.0 / 1.2, 1.0 / 1.2);
			m_scale = 1.0 / 1.2;
		}
		
		// ��scene����ת��Ϊ�Ŵ���С�������;
		QPointF scenePoint = m_graphicsView->matrix().map(scenePos);
		//ͼ��������
		QGraphicsItem* pItem1 = m_graphicsView->scene()->itemAt(scenePoint, QTransform());

		if (NULL != pItem1)
		{
			QPointF fItemPoint = pItem1->mapFromScene(scenePoint);   //��ȡͼ��������

			int a = 0;
		}

		// ͨ������������view�Ŵ���С���չʾscene��λ��;
		m_graphicsView->horizontalScrollBar()->setValue(int(scenePoint.x() - viewWidth * hScale));
		m_graphicsView->verticalScrollBar()->setValue(int(scenePoint.y() - viewHeight * vScale));
    }
}

double GraphicsViewTool::GetScale()
{
	return m_scale;
}
