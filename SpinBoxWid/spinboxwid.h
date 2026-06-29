#ifndef SPINBOXWID_H
#define SPINBOXWID_H

#include <QWidget>

class QQuickWidget;

class SpinBoxWid : public QWidget
{
	Q_OBJECT

public:
	explicit SpinBoxWid(QWidget *parent = nullptr);
	virtual ~SpinBoxWid();

public slots:
	//void fuckOffSlot(int wid, int height);
	void startScanning();
	void onAngleInputVisChanged(bool isVisuble);

private:
	QQuickWidget* m_startScanning{nullptr};

};
#endif // SPINBOXWID_H
