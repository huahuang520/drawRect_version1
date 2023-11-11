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
        font.setPixelSize(12); //设置字体大小

        QPainter painter(m_graphicsView->viewport());
        painter.begin(m_graphicsView); //绘画开始
        painter.setBrush(QBrush(QColor(0, 0, 200, 20)));  //画刷填充框内颜色
        painter.setPen(pen);
        painter.setFont(font);
        //roi图元坐标转视图坐标,因为画roi是按视图坐标画的
        QPoint roiViewPoint = m_graphicsView->mapFromScene(m_rois[i].x(), m_rois[i].y());
        QRect roi(roiViewPoint.x(), roiViewPoint.y(), m_rois[i].width(), m_rois[i].height());

        //QPixmap rawImg = m_pixmap.scaled(m_graphicsView->size(), Qt::KeepAspectRatio);
       // QPixmap rawImg = m_pixmap;
        QString strPoint = QString("X:%0, Y:%1").arg(m_rois[i].x()).arg(m_rois[i].y());           //roi位置信息
        QString strSize = QString("W:%0, H:%1").arg(m_rois[i].width()).arg(m_rois[i].height());   //roi大小信息
        std::map<int, QRect>::iterator it = m_rois.begin();
        for (int j = 0; j < i; j++)
        {
            it++;
        }
        QString roiIndex = QString("#roi:%0").arg(it->first); //roi序号

        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 5, strSize);
        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 18, strPoint);
        painter.drawText(roi.topLeft().x(), roi.topLeft().y() - 31, roiIndex);

        // painter.drawRect(m_roiRect);
        painter.drawRect(roi);

        if (m_rois[i].width() != 0 && m_rois[i].height() != 0)
        {
            //绘制中间十字架
            QPen dashPen(Qt::white);
            dashPen.setWidth(MIDDLELINE_WIDTH);
            dashPen.setStyle(Qt::DashDotLine);
            painter.setPen(dashPen);
            //从上往下画线，获取roi的y坐标始终为线宽的上边
            painter.drawLine(roi.topLeft().x() + m_rois[i].width() / 2, roi.topLeft().y() + EDGE_WIDTH, roi.bottomRight().x() - m_rois[i].width() / 2, roi.bottomRight().y());
            //从左往右画线， 获取roi的x坐标始终是线宽的左边
            painter.drawLine(roi.topLeft().x() + EDGE_WIDTH, roi.topLeft().y() + m_rois[i].height() / 2, roi.bottomRight().x(), roi.bottomRight().y() - m_rois[i].height() / 2);

            //绘制边缘九个点
            //painter.setBrush(Qt::green);
            //pen.setWidth(0);
            //painter.setPen(pen);

            //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT); //左上角
            //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //左边中心点
            //painter.drawRect(m_roiRect.bottomLeft().x(), m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //左下角
            //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT);  //顶部中心
            //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT);  //中心点
            //painter.drawRect(m_roiRect.bottomLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //底部中心点
            //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y(), POINT_WIDTH, POINT_HEIGHT); //右上角
            //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //右边中心点
            //painter.drawRect(m_roiRect.bottomRight().x() - POINT_WIDTH, m_roiRect.bottomRight().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //右下角点
        }
        //绘画结束
        painter.end();
        m_graphicsView->viewport()->update();
    }
    
}

void DrawROI::changeROIBegin(const QPoint& point)
{
    //场景坐标
    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //图形项坐标
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    isBeginGraw = true;
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    EmDirection dir = region(itemPoint);     //获取鼠标当前的坐标在ROI什么位置
    m_currentDir = dir;
    isROIBeginChange = true;
    if (dir == DIR_MIDDLE)
    {
        //鼠标在矩形中心位置
        m_graphicsView->setCursor(Qt::ClosedHandCursor);
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bMovedPressed = true;
    }
    else if (dir == DIR_NONE)
    {
        //鼠标在矩形外部
        m_graphicsView->setCursor(Qt::ArrowCursor);
        m_bPainterPressed = true;
        m_paintStartPoint.setX(itemPoint.x());
        m_paintStartPoint.setY(itemPoint.y());

        m_currentROIIndex = m_rois.size(); //更新roi序号
    }
    else
    {
        //鼠标在矩形边缘
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
    //图形项坐标
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;

    }
    else
    {
        //绘制roi超过图像边界进行处理
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
        //正在绘制状态
        paintRect(itemPoint);

    }
    else if (m_bMovedPressed)
    {
        //判断拖动roi是否超过图像边界


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
   
        //正在移动状态
        moveRect(itemPoint);
    }
    else if (m_bScalePressed)
    {
        //正在缩放大小状态
        scaleRect(itemPoint);
    }
    m_graphicsView->viewport()->update();
}

void DrawROI::changeROIEnd(const QPoint& point)
{
    if (!isBeginGraw) return;

    QPointF fScenePoint = m_graphicsView->mapToScene(point);
    //图形项坐标
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

        //判断鼠标是否在矩形中
        if (m_rois[m_currentROIIndex].contains(itemPoint))
        {
            //松开鼠标前是否正在拖放
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

    //绘制roi时只在界面点击了下便松开鼠标，这样不算绘制了新的roi
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
    //图形项坐标
    QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(fScenePoint, QTransform());

    

    if (pItem == NULL)
    {
        m_graphicsView->setCursor(Qt::ArrowCursor);
        return;
    }

        
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    //根据鼠标的位置设置当前的鼠标形状
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
    if (Qt::CTRL == event->modifiers())   //键盘CTRL键
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
                //左上角
                m_graphicsView->setCursor(Qt::SizeFDiagCursor);
                dir = DIR_LEFTTOP;
            }
            else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
            {
                //右上角
                m_graphicsView->setCursor(Qt::SizeBDiagCursor);
                dir = DIR_RIGHTTOP;
            }
            else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
            {
                //左下角
                m_graphicsView->setCursor(Qt::SizeBDiagCursor);
                dir = DIR_LEFTBOTTOM;
            }
            else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
            {
                //右下角
                m_graphicsView->setCursor(Qt::SizeFDiagCursor);
                dir = DIR_RIGHTBOTTOM;
            }
            else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
            {
                //右边
                m_graphicsView->setCursor(Qt::SizeHorCursor);
                dir = DIR_RIGHT;
            }
            else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
            {
                //上边
                m_graphicsView->setCursor(Qt::SizeVerCursor);
                dir = DIR_TOP;
            }
            else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
            {
                //下边
                m_graphicsView->setCursor(Qt::SizeVerCursor);
                dir = DIR_BOTTOM;
            }
            else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
            {
                //左边
                m_graphicsView->setCursor(Qt::SizeHorCursor);
                dir = DIR_LEFT;
            }
            else if (m_rois[i].contains(point))
            {
                //中间
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
    //    //左上角
    //    m_graphicsView->setCursor(Qt::SizeFDiagCursor);
    //    dir = DIR_LEFTTOP;
    //}
    //else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    //{
    //    //右上角
    //    m_graphicsView->setCursor(Qt::SizeBDiagCursor);
    //    dir = DIR_RIGHTTOP;
    //}
    //else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    //{
    //    //左下角
    //    m_graphicsView->setCursor(Qt::SizeBDiagCursor);
    //    dir = DIR_LEFTBOTTOM;
    //}
    //else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    //{
    //    //右下角
    //    m_graphicsView->setCursor(Qt::SizeFDiagCursor);
    //    dir = DIR_RIGHTBOTTOM;
    //}
    //else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    //{
    //    //右边
    //    m_graphicsView->setCursor(Qt::SizeHorCursor);
    //    dir = DIR_RIGHT;
    //}
    //else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    //{
    //    //上边
    //    m_graphicsView->setCursor(Qt::SizeVerCursor);
    //    dir = DIR_TOP;
    //}
    //else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    //{
    //    //下边
    //    m_graphicsView->setCursor(Qt::SizeVerCursor);
    //    dir = DIR_BOTTOM;
    //}
    //else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    //{
    //    //左边
    //    m_graphicsView->setCursor(Qt::SizeHorCursor);
    //    dir = DIR_LEFT;
    //}
    //else if (m_roiRect.contains(point))
    //{
    //    //中间
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
    m_graphicsView->setCursor(Qt::ArrowCursor);                    //设置鼠标为指针形状

    int width = mousePoint.x() - m_paintStartPoint.x();  //移动的宽度
    int height = mousePoint.y() - m_paintStartPoint.y(); //移动的高度

    //单个roi时，把多余的roi给消掉
    if (!m_isDrawMultipROIS)
    {
        m_currentROIIndex = 0;
        for (int i = 1; i < m_rois.size(); i++)
        {
            m_rois[i] = QRect(0 ,0 ,0, 0);
            m_rois.erase(i);
        }
    }

    //确定矩形左上角坐标
    if (width < 0 && height < 0)
    {
        //鼠标向左上角移动
        m_rois[m_currentROIIndex].setX(mousePoint.x());
        m_rois[m_currentROIIndex].setY(mousePoint.y());
    }
    else if (width < 0)
    {
        //鼠标向左下位置移动
        m_rois[m_currentROIIndex].setX(mousePoint.x());
        m_rois[m_currentROIIndex].setY(m_paintStartPoint.y());
    }
    else if (height < 0)
    {
        //鼠标向右上位置移动
        m_rois[m_currentROIIndex].setX(m_paintStartPoint.x());
        m_rois[m_currentROIIndex].setY(mousePoint.y());
    }
    else
    {
        //正常  向右下移动
        m_rois[m_currentROIIndex].setX(m_paintStartPoint.x());
        m_rois[m_currentROIIndex].setY(m_paintStartPoint.y());
    }

    //设置矩形大小 绝对值 避免反方向的产生的负值
    m_rois[m_currentROIIndex].setSize(QSize(abs(width) + 1, abs(height) + 1)); //+1是因为假如宽高都为265，则像素坐标范围为[0, 264] 
}

void DrawROI::moveRect(const QPoint& mousePoint)
{
    m_graphicsView->setCursor(Qt::ClosedHandCursor);

    int width = mousePoint.x() - m_moveStartPoint.x();
    int height = mousePoint.y() - m_moveStartPoint.y();

    //如果roi边界超过的图像边界进行处理
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
    int width = mousePoint.x() - m_moveStartPoint.x();   //移动的宽度
    int height = mousePoint.y() - m_moveStartPoint.y();  //移动的高度

    //根据当前的缩放状态来改变矩形的位置大小信息
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
        //缩放的大小限制
        return;
    }

    m_rois[m_currentROIIndex] = newRect;
    m_moveStartPoint = mousePoint;  //更新鼠标的起始位置
}
