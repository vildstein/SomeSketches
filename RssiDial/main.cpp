#include "paintwidget.h"
#include "RaRssiSnrEmuWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PaintWidget w;
	RaRssiSnrEmuWidget wid;

	QObject::connect(&wid, &RaRssiSnrEmuWidget::proceedRssiSignal, &w, &PaintWidget::someThingChanged);
	QObject::connect(&w, &PaintWidget::positionChanged, &wid, &RaRssiSnrEmuWidget::setCurrentDirectionAngle);

	wid.show();
	w.show();

	auto result = a.exec();
	return result;
}
