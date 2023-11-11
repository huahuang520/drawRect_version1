#include "DrawROI.h"

DrawROI::DrawROI(QGraphicsView* GraphicsView, QObject*parent)
	: QObject(parent)
{
    m_graphicsView = GraphicsView;
    m_isDrawMultipROIS = false;
    isROIBeginChange = false;
    m_currentROIIndex = 0;
    initViewer();
}

DrawROI::~DrawROI()
{
}

void DrawROI::drawROI()
{
    if (m_pixmap.isNull())
        return;

    for (int i = 0; i < m_rois.size(); i++)
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidth(EDGE_WIDTH);

        QFont font;
        font.setPixelSize(12); //���������С

        QPainter painter(m_graphicsView->viewport());
        painter.begin(m_graphicsView); //�滭��ʼ
        painter.setBrush(QBrush(QColor(0, 0, 200, 20)));  //��ˢ��������ɫ
        painter.setPen(pen);
        painter.setFont(font);
        //roiͼԪ����ת��ͼ����,��Ϊ��roi�ǰ���ͼ���껭��
        QPoint roiViewPoint = m_graphicsView->mapFromScene(m_rois[i].x(), m_rois[i].y());
        QRect roi(roiViewPoint.x(), roiViewPoint.y(), m_rois[i].width(), m_rois[i].height());

        //QPixmap rawImg = m_pixmap.scaled(m_graphicsView->size(), Qt::KeepAspectRatio);
       // QPixmap rawImg = m_pixmap;
        QString strPoint = QString("X:%0, Y:%1").arg(m_rois[i].x()).arg(m_rois[i].y());           //roiλ����Ϣ
        QString strSize = QString("W:%0, H:%1").arg(m_rois[i].width()).arg(m_rois[i].height());   //roi��С��Ϣ
        std::map<int, QRect>::iterator it = m_rois.begin();
        for (int j = 0; j < i; j++)
        {
            it++;
        }
        QString roiIndex = QString("#roi:%0").arg(it->first); //roi���

        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 5, strSize);
        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 18, strPoint);
        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 31, roiIndex);

        // painter.drawRect(m_roiRect);
        painter.drawRect(roi);

        if (m_rois[i].width() != 0 && m_rois[i].height() != 0)
        {
            //�����м�ʮ�ּ�
            QPen dashPen(Qt::white);
            dashPen.setWidth(MIDDLELINE_WIDTH);
            dashPen.setStyle(Qt::DashDotLine);
            painter.setPen(dashPen);
            //�������»��ߣ���ȡroi��y����ʼ��Ϊ�߿���ϱ�
            painter.drawLine(roi.topLeft().x() + m_rois[i].width() / 2, roi.topLeft().y() + EDGE_WIDTH, roi.bottomRight().x() - m_rois[i].width() / 2, roi.bottomRight().y());
            //�������һ��ߣ� ��ȡroi��x����ʼ�����߿�����
            painter.drawLine(roi.topLeft().x() + EDGE_WIDTH, roi.topLeft().y() + m_rois[i].height() / 2, roi.bottomRight().x(), roi.bottomRight().y() - m_rois[i].height() / 2);

            //���Ʊ�Ե�Ÿ���
            //painter.setBrush(Qt::green);
            //pen.setWidth(0);
            //painter.setPen(pen);

            //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT); //���Ͻ�
            //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //������ĵ�
            //painter.drawRect(m_roiRect.bottomLeft().x(), m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //���½�
            //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT);  //��������
            //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT);  //���ĵ�
            //painter.drawRect(m_roiRect.bottomLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //�ײ����ĵ�
            //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y(), POINT_WIDTH, POINT_HEIGHT); //���Ͻ�
            //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //�ұ����ĵ�
            //painter.drawRect(m_roiRect.bottomRight().x() - POINT_WIDTH, m_roiRect.bottomRight().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //���½ǵ�
        }
        //�滭����
        painter.end();
        m_graphicsView->viewport()->update();
    }
    
}

void DrawROI::changeROIBegin(const QPoint& point)
{
    //��������
    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    isBeginGraw = true;
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    EmDirection dir = region(itemPoint);     //��ȡ��굱ǰ��������ROIʲôλ��
    m_currentDir = dir;
    isROIBeginChange = true;
    if (dir == DIR_MIDDLE)
    {
        //����ھ�������λ��
        m_graphicsView->setCursor(Qt::ClosedHandCursor);
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bMovedPressed = true;
    }
    else if (dir == DIR_NONE)
    {
        //����ھ����ⲿ
        m_graphicsView->setCursor(Qt::ArrowCursor);
        m_bPainterPressed = true;
        m_paintStartPoint.setX(itemPoint.x());
        m_paintStartPoint.setY(itemPoint.y());

        m_currentROIIndex = m_rois.size(); //����roi���
    }
    else
    {
        //����ھ��α�Ե
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bScalePressed = true;
        m_emCurDir = dir;
    }
}

void DrawROI::changeROIInProgress(const QPoint& point)
{
    if (!isBeginGraw) return;

    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;

    }
    else
    {
        //����roi����ͼ��߽���д���
        if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        else if (fScenePoint.x() < 0)
        {
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        else if (fScenePoint.y() < 0)
        {
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        else if (fScenePoint.x() >= m_pixmap.width() && fScenePoint.y() >= m_pixmap.height())
        {
            QPoint item(m_pixmap.width() - 1, m_pixmap.height() - 1);
            itemPoint = item;
        }
        else if (fScenePoint.x() >= m_pixmap.width())
        {
            QPoint item(m_pixmap.width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        else if (fScenePoint.y() >= m_pixmap.height())
        {
            QPoint item(fScenePoint.x(), m_pixmap.height() - 1);
            itemPoint = item;
        }
    }
    
    if (m_bPainterPressed)
    {
        //���ڻ���״̬
        paintRect(itemPoint);

    }
    else if (m_bMovedPressed)
    {
        //�ж��϶�roi�Ƿ񳬹�ͼ��߽�


        if ((m_rois[m_currentROIIndex].x() <= 0 && m_rois[m_currentROIIndex].y() <= 0))
        {
            m_rois[m_currentROIIndex].setX(0);
            m_rois[m_currentROIIndex].setY(0);
        }
        else if ((m_rois[m_currentROIIndex].x() <= 0))
        {
            m_rois[m_currentROIIndex].setX(0);
        }
        else if ((m_rois[m_currentROIIndex].y() <= 0))
        {
            m_rois[m_currentROIIndex].setY(0);
        }
        else if ((m_rois[m_currentROIIndex].bottomRight().x() >= m_pixmap.width() - 1 && m_rois[m_currentROIIndex].bottomRight().y() >= m_pixmap.height() - 1))
        {
            m_rois[m_currentROIIndex].bottomRight().setX(m_pixmap.width() - 1);
            m_rois[m_currentROIIndex].bottomRight().setY(m_pixmap.height() - 1);
        }
        else if ((m_rois[m_currentROIIndex].bottomRight().x() >= m_pixmap.width() - 1))
        {            
            m_rois[m_currentROIIndex].bottomRight().setX(m_pixmap.width() - 1);
        }
        else if ((m_rois[m_currentROIIndex].bottomRight().y() >= m_pixmap.height() - 1))
        {
            m_rois[m_currentROIIndex].bottomRight().setY(m_pixmap.height() - 1);
        }
   
        //�����ƶ�״̬
        moveRect(itemPoint);
    }
    else if (m_bScalePressed)
    {
        //�������Ŵ�С״̬
        scaleRect(itemPoint);
    }
    m_graphicsView->viewport()->update();
}

void DrawROI::changeROIEnd(const QPoint& point)
{
    if (!isBeginGraw) return;

    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

        //�ж�����Ƿ��ھ�����
        if (m_rois[m_currentROIIndex].contains(itemPoint))
        {
            //�ɿ����ǰ�Ƿ������Ϸ�
            if (m_bMovedPressed)
            {
                m_graphicsView->setCursor(Qt::OpenHandCursor);
            }
            else
            {
                m_graphicsView->setCursor(Qt::ArrowCursor);
            }
        }
    }

    //����roiʱֻ�ڽ��������±��ɿ���꣬��������������µ�roi
    if (m_rois[m_currentROIIndex].width() == 0 || m_rois[m_currentROIIndex].height() == 0)
    {
        m_rois[m_currentROIIndex] = QRect(0, 0, 0, 0);
        m_rois.erase(m_currentROIIndex);
        m_currentROIIndex -= 1;
    }
    
    for (int i = 0; i < m_rois.size(); i++)
    {
        m_OriginRois[i] = m_rois[i];
    }

    m_paintStartPoint = QPoint();
    m_bMovedPressed = false;
    m_bPainterPressed = false;
    m_bScalePressed = false;
    isROIBeginChange = false;
}

void DrawROI::changeMouseShape(const QPoint& point)
{
    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    

    if (pItem == NULL)
    {
        m_graphicsView->setCursor(Qt::ArrowCursor);
        return;
    }

        
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    //��������λ�����õ�ǰ�������״
    EmDirection dir = region(itemPoint);

    if (dir == DIR_NONE)
    {
        m_graphicsView->setCursor(Qt::ArrowCursor);
    }
    else if (dir == DIR_TOP || dir == DIR_BOTTOM)
    {
        m_graphicsView->setCursor(Qt::SizeVerCursor);
    }
    else if (dir == DIR_LEFT || dir == DIR_RIGHT)
    {
        m_graphicsView->setCursor(Qt::SizeHorCursor);
    }
    else if (dir == DIR_LEFTTOP || dir == DIR_RIGHTBOTTOM)
    {
        m_graphicsView->setCursor(Qt::SizeFDiagCursor);
    }
    else if (dir == DIR_LEFTBOTTOM || dir == DIR_RIGHTTOP)
    {
        m_graphicsView->setCursor(Qt::SizeBDiagCursor);
    }
    else if (dir == DIR_MIDDLE && !m_bMovedPressed)
    {
        m_graphicsView->setCursor(Qt::OpenHandCursor);
    }
    else if (dir == DIR_MIDDLE && m_bMovedPressed)
    {
        m_graphicsView->setCursor(Qt::ClosedHandCursor);
    }
}

std::map<int, QRect> DrawROI::getRoiRect() const
{
    return m_rois;
}

void DrawROI::DeleteCurrentROI()
{
    m_rois[m_currentROIIndex] = QRect(0, 0, 0, 0);
    m_rois.erase(m_currentROIIndex);
    
    int index = 0;
    for (std::map<int, QRect>::iterator it = m_rois.begin(); it != m_rois.end(); it++)
    {
        m_rois[index] = it->second;
        index++;
    }
    int size = m_rois.size();
    if (m_currentROIIndex == size)
    {
        m_currentROIIndex = -1;
        return;
    }
    m_currentROIIndex = -1;

    m_rois[size - 1] = QRect(0, 0, 0, 0);
    m_rois.erase(size - 1);
}

void DrawROI::saveAllROIImage()
{
    /*QString fileName = QFileDialog::getSaveFileName(m_graphicsView, "Save", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "*.png");
    if (fileName.isEmpty())
    {
        return;
    }

    QImage saveImg = m_pixmap.toImage().copy(m_roiRect);
    saveImg.save(fileName);*/
    std::map<int, QRect>::iterator it = m_rois.begin();
    for (int i = 0; i < m_rois.size(); i++)
    {
        QImage saveImg = m_pixmap.toImage().copy(m_rois[i]);
        std::string roiName = "#roi" + std::to_string(it->first) + ".png";
        saveImg.save("C:\\Users\\hua.huang\\Desktop\\roi\\" + QString::fromStdString(roiName));
        it++;
    }
}

void DrawROI::saveCurrentROIImage()
{
    if (m_currentROIIndex == -1)
    {
        return;
    }

    QImage saveImg = m_pixmap.toImage().copy(m_rois[m_currentROIIndex]);
    std::map<int, QRect>::iterator it = m_rois.begin();
    for (int i = 0; i < m_currentROIIndex; i++)
    {
        it++;
    }
    std::string roiName = "#roi" + std::to_string(it->first) + ".png";
    saveImg.save("C:\\Users\\hua.huang\\Desktop\\roi\\" + QString::fromStdString(roiName));
}

//void DrawROI::SetBackImage(QImage* img)
//{
//    m_image = *img;
//}

void DrawROI::SetBackPix(QPixmap& pix)
{
    m_pixmap = pix;
    m_graphicsView->viewport()->update();
}

void DrawROI::isDrawMultipleROIs(bool isMultioROIS)
{
    m_isDrawMultipROIS = isMultioROIS;
}

void DrawROI::ScaleROIS(QWheelEvent* event, double scale)
{
    if (Qt::CTRL == event->modifiers())   //����CTRL��
    {
        int size = m_rois.size();
        
        for (int i = 0; i < size; i++)
        {
            double width = m_OriginRois[i].width() * scale;
            double height = m_OriginRois[i].height() * scale;
            m_OriginRois[i] = QRectF(m_OriginRois[i].x(), m_OriginRois[i].y(), width, height);

            int x = m_rois[i].x();
            int y = m_rois[i].y();
            
            m_rois[i] = QRect(x, y, width, height);
        }
    }
    
}

void DrawROI::DeleteAllROIS()
{
    int size = m_rois.size();
    if (size == 0) return;
    for (int i = 0; i < size; i++)
    {
        m_rois[i] = QRect(0, 0, 0, 0);
        m_rois.erase(i);
    }
    m_currentROIIndex = -1;
}

void DrawROI::initViewer()
{
    m_bPainterPressed = false;
    m_bMovedPressed = false;
    m_bScalePressed = false;
    //m_rois[0] = QRect(0, 0, 0, 0);
    m_emCurDir = EmDirection::DIR_NONE;

    m_graphicsView->setMouseTracking(true);
    m_graphicsView->setFocusPolicy(Qt::StrongFocus);
}

EmDirection DrawROI::region(const QPoint& point)
{
    int mouseX = point.x();
    int mouseY = point.y();

    int roiSum = m_rois.size();
    if (roiSum == 0)
    {
        return DIR_NONE;
    }
    else
    {
        if (!m_isDrawMultipROIS)
        {
            for (int i = 1; i < roiSum; i++)
            {
                QRect rect(0,0,0,0);
                m_rois[i] = rect;
                m_rois.erase(i);
            }
            roiSum = 1;
            m_currentROIIndex = 0;
        }

        for (int i = 0; i < roiSum; i++)
        {
            QPoint roiTopLeft = m_rois[i].topLeft();
            QPoint roiBottomRight = m_rois[i].bottomRight();

            EmDirection dir = DIR_NONE;

            if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
            {
                //���Ͻ�
                m_graphicsView->setCursor(Qt::SizeFDiagCursor);
                dir = DIR_LEFTTOP;
            }
            else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
            {
                //���Ͻ�
                m_graphicsView->setCursor(Qt::SizeBDiagCursor);
                dir = DIR_RIGHTTOP;
            }
            else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
            {
                //���½�
                m_graphicsView->setCursor(Qt::SizeBDiagCursor);
                dir = DIR_LEFTBOTTOM;
            }
            else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
            {
                //���½�
                m_graphicsView->setCursor(Qt::SizeFDiagCursor);
                dir = DIR_RIGHTBOTTOM;
            }
            else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
            {
                //�ұ�
                m_graphicsView->setCursor(Qt::SizeHorCursor);
                dir = DIR_RIGHT;
            }
            else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
            {
                //�ϱ�
                m_graphicsView->setCursor(Qt::SizeVerCursor);
                dir = DIR_TOP;
            }
            else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
            {
                //�±�
                m_graphicsView->setCursor(Qt::SizeVerCursor);
                dir = DIR_BOTTOM;
            }
            else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
            {
                //���
                m_graphicsView->setCursor(Qt::SizeHorCursor);
                dir = DIR_LEFT;
            }
            else if (m_rois[i].contains(point))
            {
                //�м�
                dir = DIR_MIDDLE;
            }
            else
            {
                dir = DIR_NONE;
            }
            if (dir != DIR_NONE && !isROIBeginChange)
            {
                m_currentROIIndex = i;
                return dir;
            }
            else if (dir != DIR_NONE)
            {
                return m_currentDir;
            }
        }

        return DIR_NONE;
    }
    

    //QPoint roiTopLeft = m_roiRect.topLeft();
    //QPoint roiBottomRight = m_roiRect.bottomRight();

    //EmDirection dir = DIR_NONE;

    //if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    //{
    //    //���Ͻ�
    //    m_graphicsView->setCursor(Qt::SizeFDiagCursor);
    //    dir = DIR_LEFTTOP;
    //}
    //else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    //{
    //    //���Ͻ�
    //    m_graphicsView->setCursor(Qt::SizeBDiagCursor);
    //    dir = DIR_RIGHTTOP;
    //}
    //else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    //{
    //    //���½�
    //    m_graphicsView->setCursor(Qt::SizeBDiagCursor);
    //    dir = DIR_LEFTBOTTOM;
    //}
    //else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    //{
    //    //���½�
    //    m_graphicsView->setCursor(Qt::SizeFDiagCursor);
    //    dir = DIR_RIGHTBOTTOM;
    //}
    //else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    //{
    //    //�ұ�
    //    m_graphicsView->setCursor(Qt::SizeHorCursor);
    //    dir = DIR_RIGHT;
    //}
    //else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    //{
    //    //�ϱ�
    //    m_graphicsView->setCursor(Qt::SizeVerCursor);
    //    dir = DIR_TOP;
    //}
    //else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    //{
    //    //�±�
    //    m_graphicsView->setCursor(Qt::SizeVerCursor);
    //    dir = DIR_BOTTOM;
    //}
    //else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    //{
    //    //���
    //    m_graphicsView->setCursor(Qt::SizeHorCursor);
    //    dir = DIR_LEFT;
    //}
    //else if (m_roiRect.contains(point))
    //{
    //    //�м�
    //    dir = DIR_MIDDLE;
    //}
    //else
    //{
    //    dir = DIR_NONE;
    //}
    //return dir;
}

void DrawROI::paintRect(const QPoint& mousePoint)
{
    m_graphicsView->setCursor(Qt::ArrowCursor);                    //�������Ϊָ����״

    int width = mousePoint.x() - m_paintStartPoint.x();  //�ƶ��Ŀ��
    int height = mousePoint.y() - m_paintStartPoint.y(); //�ƶ��ĸ߶�

    //����roiʱ���Ѷ����roi������
    if (!m_isDrawMultipROIS)
    {
        m_currentROIIndex = 0;
        for (int i = 1; i < m_rois.size(); i++)
        {
            m_rois[i] = QRect(0 ,0 ,0, 0);
            m_rois.erase(i);
        }
    }

    //ȷ���������Ͻ�����
    if (width < 0 && height < 0)
    {
        //��������Ͻ��ƶ�
        m_rois[m_currentROIIndex].setX(mousePoint.x());
        m_rois[m_currentROIIndex].setY(mousePoint.y());
    }
    else if (width < 0)
    {
        //���������λ���ƶ�
        m_rois[m_currentROIIndex].setX(mousePoint.x());
        m_rois[m_currentROIIndex].setY(m_paintStartPoint.y());
    }
    else if (height < 0)
    {
        //���������λ���ƶ�
        m_rois[m_currentROIIndex].setX(m_paintStartPoint.x());
        m_rois[m_currentROIIndex].setY(mousePoint.y());
    }
    else
    {
        //����  �������ƶ�
        m_rois[m_currentROIIndex].setX(m_paintStartPoint.x());
        m_rois[m_currentROIIndex].setY(m_paintStartPoint.y());
    }

    //���þ��δ�С ����ֵ ���ⷴ����Ĳ����ĸ�ֵ
    m_rois[m_currentROIIndex].setSize(QSize(abs(width) + 1, abs(height) + 1)); //+1����Ϊ�����߶�Ϊ265�����������귶ΧΪ[0, 264] 
}

void DrawROI::moveRect(const QPoint& mousePoint)
{
    m_graphicsView->setCursor(Qt::ClosedHandCursor);

    int width = mousePoint.x() - m_moveStartPoint.x();
    int height = mousePoint.y() - m_moveStartPoint.y();

    //���roi�߽糬����ͼ��߽���д���
    if ((m_rois[m_currentROIIndex].x() + width + m_rois[m_currentROIIndex].width() > m_pixmap.width() && m_rois[m_currentROIIndex].y() + height + m_rois[m_currentROIIndex].height() > m_pixmap.height())
        || (m_rois[m_currentROIIndex].x() + width + m_rois[m_currentROIIndex].width() > m_pixmap.width()) || (m_rois[m_currentROIIndex].y() + height + m_rois[m_currentROIIndex].height() > m_pixmap.height())
        || (m_rois[m_currentROIIndex].x() + width < 0) || (m_rois[m_currentROIIndex].y() + height < 0))
    {
        m_moveStartPoint = mousePoint;
        return;
    }

    QRect ret;
    ret.setX(m_rois[m_currentROIIndex].x() + width);
    ret.setY(m_rois[m_currentROIIndex].y() + height);
    ret.setSize(m_rois[m_currentROIIndex].size());
    m_rois[m_currentROIIndex] = ret;
    m_moveStartPoint = mousePoint;
}

void DrawROI::scaleRect(const QPoint& mousePoint)
{
    QRect newRect(m_rois[m_currentROIIndex].topLeft(), m_rois[m_currentROIIndex].bottomRight());
    int width = mousePoint.x() - m_moveStartPoint.x();   //�ƶ��Ŀ��
    int height = mousePoint.y() - m_moveStartPoint.y();  //�ƶ��ĸ߶�

    //���ݵ�ǰ������״̬���ı���ε�λ�ô�С��Ϣ
    switch (m_emCurDir)
    {
    case DIR_LEFT:
        newRect.setLeft(mousePoint.x());
        break;
    case DIR_RIGHT:
        newRect.setRight(mousePoint.x());
        break;
    case DIR_TOP:
        newRect.setTop(mousePoint.y());
        break;
    case DIR_BOTTOM:
        newRect.setBottom(mousePoint.y());
        break;
    case DIR_LEFTTOP:
        newRect.setTopLeft(mousePoint);
        break;
    case DIR_LEFTBOTTOM:
        newRect.setBottomLeft(mousePoint);
        break;
    case DIR_RIGHTTOP:
        newRect.setTopRight(mousePoint);
        break;
    case DIR_RIGHTBOTTOM:
        newRect.setBottomRight(mousePoint);
        break;
    }

    if (newRect.width() < MIN_WIDTH || newRect.height() < MIN_HEIGHT)
    {
        //���ŵĴ�С����
        return;
    }

    m_rois[m_currentROIIndex] = newRect;
    m_moveStartPoint = mousePoint;  //����������ʼλ��
}
