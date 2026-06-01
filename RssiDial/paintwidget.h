#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>


//class QPainter;

class PaintWidget : public QWidget
{
	Q_OBJECT

public:
	PaintWidget(QWidget *parent = nullptr);
	virtual ~PaintWidget();

	void someThingChanged(qint32 rssi, qint32 snr);
	void onAzimuthChanged(int az);

signals:
	void positionChanged(const QPoint& pos);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;


private:
	QVector<QRect> createRectVector();
	void countSimpleMA(int period);

	void createLine(const QVector<QRect>& rect);


	QPolygon m_redLinePoligon;

	QPolygon m_MA;

	QRect m_rect;

	QLine m_mALine;

	QVector<QRect> m_rectVec;

	int m_az{0};

	bool m_isMA_shown{false};

	//Q

};
#endif // PAINTWIDGET_H
