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


private:
	QVector<QRect> createRectVector();
	QPolygon m_pol;
	QRect m_rect;

	QVector<QRect> m_rectVec;

	int m_az{0};

	//Q

};
#endif // PAINTWIDGET_H
