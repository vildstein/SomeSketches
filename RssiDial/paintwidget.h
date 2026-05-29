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

signals:
	void positionChanged(const QPoint& pos);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;

private:

	QPolygon m_pol;
	QRect m_rect;

};
#endif // PAINTWIDGET_H
