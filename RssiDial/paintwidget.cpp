#include "paintwidget.h"

#include <QPainter>
#include <QTime>
#include <QMoveEvent>

#include <QtConcurrent>

#include <QDebug>

#include <QFuture>
#include <QFutureWatcher>

constexpr const int rectWidth{15};
constexpr const int rectInitialHeigth{10};
constexpr const int betweenRectDistance{20};

PaintWidget::PaintWidget(QWidget *parent)
	: QWidget(parent)//, m_rect(0, 0, 40, 80)
{

	// fm_pol << QPoint{0,0} << QPoint{5,5} << QPoint{10,8} << QPoint{15,15} << QPoint{20,15} << QPoint{25,15};

	//m_rect = QRect(20, 40, 50, -80);

	QFutureWatcher<decltype(m_rectVec)>* fw = new QFutureWatcher<decltype(m_rectVec)>(this);

	auto getVectorLabda = [this, fw](){
		fw->waitForFinished();
		auto vec(fw->future().result());
		if (!vec.empty()) {
			m_rectVec = vec;
		}
		fw->deleteLater();
	};

	connect(fw, &QFutureWatcher<decltype(m_rectVec)>::finished, this, getVectorLabda);

	auto future = QtConcurrent::run(this, &PaintWidget::createRectVector);
	fw->setFuture(future);
}

PaintWidget::~PaintWidget() = default;


void PaintWidget::someThingChanged(qint32 rssi, qint32 snr)
{
	auto rss = -rssi;
	//auto& rect = m_rectVec[4];
	//m_rect.setHeight(rss);
	m_rectVec[m_az].setHeight(rss);
	update();
}

void PaintWidget::onAzimuthChanged(int az)
{

	if (az < 359) {
		m_az = az;
	} else {
		m_az = 0;
	}

}


void PaintWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Перенеосим на 10 пикселей в левый нижий угол
	painter.translate(20, height() - 20);

	// ОСИ
	QColor blackColor(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(blackColor);

	{
		QLine xAxis({-10, 0}, {width() - 35, 0});
		QLine yAxis({0, 10}, {0, -height() + 35});

		painter.drawLine(xAxis);
		painter.drawLine(yAxis);
	}
	// END ОСИ

	// Прямоугольники
	painter.save();
	QColor blueColor(Qt::blue);
	painter.setPen(blackColor);
	painter.setBrush(blueColor);
	painter.translate(rectWidth, 0);
	painter.rotate(180);
	painter.drawRects(m_rectVec);
	painter.restore();

	// END Прямоугольники










	//int side = qMin(width(), height());
	// Перенеосим рисование в середину виджета
	//painter.translate(width() / 2, height() / 2);

	//painter.save();


	//painter.translate(10, 10);


	// Приближам изображение
	//painter.scale(side / 200.0, side / 200.0);



	//QColor blueColor(Qt::blue);


	//







}

void PaintWidget::moveEvent(QMoveEvent* event)
{
	auto point =  event->pos();
	emit positionChanged(point);
}

QVector<QRect> PaintWidget::createRectVector()
{
	QVector<QRect> vec;
	int xCoord{0};

	for (size_t i = 0; i < 359; ++i) {

		QRect rect(xCoord, 0, rectWidth, rectInitialHeigth);
		vec.push_back(rect);
		xCoord -= betweenRectDistance;
	}

	return vec;
}

