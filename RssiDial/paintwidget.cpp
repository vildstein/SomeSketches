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
constexpr const int EMA_period{4};


PaintWidget::PaintWidget(QWidget *parent)
	: QWidget(parent)//, m_rect(0, 0, 40, 80)
{
	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


	QFutureWatcher<decltype(m_rectVec)>* fw = new QFutureWatcher<decltype(m_rectVec)>(this);

	auto getVectorLabda = [this, fw](){
		fw->waitForFinished();
		auto vec(fw->future().result());
		if (!vec.empty()) {
			m_rectVec = vec;
		}
		createLine(vec);
		countSimpleMA(EMA_period);
		fw->deleteLater();
	};

	connect(fw, &QFutureWatcher<decltype(m_rectVec)>::finished, this, getVectorLabda);

	auto future = QtConcurrent::run(this, &PaintWidget::createRectVector);
	fw->setFuture(future);

}

PaintWidget::~PaintWidget() = default;


void PaintWidget::someThingChanged(qint32 rssi, qint32 snr)
{
	auto rss{100};
	rss += rssi;
	m_rectVec[m_az].setHeight(rss);
	m_redLinePoligon[m_az].setY(m_rectVec[m_az].height());
	countSimpleMA(EMA_period);
	update();
	qInfo() << rssi;
	qInfo() << rss;
}

void PaintWidget::onAzimuthChanged(int az)
{
	if (az < 360) {
		m_az = az;
	} else {
		m_az = 0;
	}
	update();
}

void PaintWidget::setBoardAz(int azVal)
{
	m_boardAz = azVal;
	update();
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
	// END Прямоугольники


	if (m_isMA_shown == true) {
		QPen pen(QBrush(Qt::yellow), 2);
		painter.setPen(pen);
		painter.drawPolyline(m_redLinePoligon);

		QPen redPen(QBrush(Qt::red), 4);
		painter.setPen(redPen);
		painter.drawPolyline(m_MA);
	}

	painter.restore();

	painter.drawText(10, -height() + 50, QString::number(m_az));

	QPen greenPen(Qt::green);
	QBrush gb(Qt::green);
	painter.setPen(greenPen);
	painter.setBrush(gb);
	painter.drawEllipse(-m_rectVec[m_boardAz].x(), -100, rectWidth, 20);
}

// void PaintWidget::moveEvent(QMoveEvent* event)
// {
// 	auto point =  event->pos();
// 	emit positionChanged(point);
// }

void PaintWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_isMA_shown = !m_isMA_shown;
	qInfo() << m_isMA_shown;
	update();
}

QVector<QRect> PaintWidget::createRectVector()
{
	QVector<QRect> vec;
	int xCoord{0};

	for (size_t i = 0; i < 360; ++i) {

		QRect rect(xCoord, 0, rectWidth, rectInitialHeigth);
		vec.push_back(rect);
		xCoord -= betweenRectDistance;
	}

	return vec;
}

void PaintWidget::createLine(const QVector<QRect>& rect)
{
	for (auto it = rect.begin(); it != rect.cend(); ++it) {
		QPoint coord{it->center().x(), it->bottomRight().y()};
		m_redLinePoligon.push_back(coord);
	}
}

void PaintWidget::countSimpleMA(int period)
{
	//QPolygon maValues;

	QVector<int> maValues;

	const auto yCoordsCount = m_redLinePoligon.size();
	int yCoords[yCoordsCount];
	//int resultArray[yCoordsCount];
	{
		size_t yCoordsIndex{0};
		for (auto it = m_redLinePoligon.cbegin(); it != m_redLinePoligon.cend(); ++it) {
			yCoords[yCoordsIndex] = it->y();
			++yCoordsIndex;
		}
	}

	for (int index = 0; index < m_rectVec.size(); ++index) {
			int result{0};

			if (index < period) {

				int count{index + 1};
				do {
					--count;
					result += ( yCoords[count] / (index + 1) );
				} while (count != 0);
				maValues.push_back( result );
			} else {
				size_t count{0};
				do {
					result += ( yCoords[ (index-count) ] / period );
					++count;
				} while (count < period);
				maValues.push_back( result );
			}
	}

	m_MA = m_redLinePoligon;

	for (int index = 0; index < m_MA.size(); ++index) {
		auto yCoord = maValues[index];
		m_MA[index].setY(yCoord);
	}
}

