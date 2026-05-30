#include "paintwidget.h"
#include "RaRssiSnrEmuWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PaintWidget* w = new PaintWidget;
	w->setAttribute(Qt::WA_DeleteOnClose, true);
	RaRssiSnrEmuWidget* wid = new RaRssiSnrEmuWidget;
	wid->setAttribute(Qt::WA_DeleteOnClose, true);

	QObject::connect(wid, &RaRssiSnrEmuWidget::proceedRssiSignal, w, &PaintWidget::someThingChanged);
	QObject::connect(w, &PaintWidget::positionChanged, wid, &RaRssiSnrEmuWidget::setCurrentDirectionAngle);
	QObject::connect(wid, &RaRssiSnrEmuWidget::azimuthChanged, w, &PaintWidget::onAzimuthChanged);

	emit wid->azimuthChanged(24);

	QObject::connect(w, &PaintWidget::destroyed, qApp, &QApplication::quit);
	QObject::connect(wid, &RaRssiSnrEmuWidget::destroyed, qApp, &QApplication::quit);

	wid->show();
	w->show();

	auto result = a.exec();
	return result;
}
