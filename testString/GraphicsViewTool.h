#pragma once

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QScrollBar>

class GraphicsViewTool : public QObject
{
	Q_OBJECT

public:
	GraphicsViewTool(QObject* parent = nullptr);
	GraphicsViewTool(QGraphicsView* graphicsView, QGraphicsScene* scene, QGraphicsPixmapItem* pixmapItem, QObject* parent = nullptr);
	~GraphicsViewTool();

public:
	void SetPixMiddle(const QImage& image); //����resizeEvent�� ����仯��֤ͼ���ڽ�������   ------����
	QPixmap& GetCurrentPix(); //��ȡ���µ�QPixmap

	void ScaleImage(QWheelEvent* event); //����wheelEvent�� ����ͼ��
	double GetScale(); //��ȡ�Ŵ���
private:
	QGraphicsView* m_graphicsView;
	QGraphicsScene* m_scene;
	QGraphicsPixmapItem* m_pixmapItem;
	QPixmap m_pixmap;
	double m_scale;
};
