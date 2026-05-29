#include "paintwidget.h"

#include <QPainter>
#include <QTime>
#include <QMoveEvent>

#include <QDebug>

PaintWidget::PaintWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pol << QPoint{0,0} << QPoint{5,5} << QPoint{10,8} << QPoint{15,15} << QPoint{20,15} << QPoint{25,15};

	m_rect = QRect(20, 40, 50, -80);


}

void PaintWidget::someThingChanged(qint32 rssi, qint32 snr)
{
	auto rss = -rssi;
	m_rect.setHeight(rss);
	update();
}

PaintWidget::~PaintWidget() = default;

void PaintWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//int side = qMin(width(), height());
	// Перенеосим рисование в середину виджета
	//painter.translate(width() / 2, height() / 2);

	//painter.save();
	// Перенеосим на 10 пикселей в левый нижий угол
	painter.translate(20, height() - 20);

	//painter.translate(10, 10);
	//painter.restore();

	// Приближам изображение
	//painter.scale(side / 200.0, side / 200.0);



	//QColor blueColor(Qt::blue);
	// ОСИ

	QColor blackColor(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(blackColor);

	QLine xAxis({-10, 0}, {width() - 35, 0});
	QLine yAxis({0, 10}, {0, -height() + 35});

	painter.drawLine(xAxis);
	painter.drawLine(yAxis);
	// END ОСИ

	//




	painter.drawPolyline(m_pol);


	painter.drawRect(m_rect);


}

void PaintWidget::moveEvent(QMoveEvent* event)
{
	auto point =  event->pos();
	emit positionChanged(point);
}
