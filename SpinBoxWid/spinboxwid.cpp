#include "spinboxwid.h"

#include <QQuickWidget>
#include <QVBoxLayout>
#include <QQuickItem>

#include <QDebug>

SpinBoxWid::SpinBoxWid(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* lay = new QVBoxLayout;
	QQuickWidget* qmlWid = new QQuickWidget;

	qmlWid->resize(410, 200);
	qmlWid->setResizeMode(QQuickWidget::SizeRootObjectToView);

	QUrl file(("qrc:/wid.qml"));
	qmlWid->setSource(file);

	lay->addWidget(qmlWid);
	setLayout(lay);

	resize(sizeHint());

	connect(qmlWid->rootObject(), SIGNAL(shitChanged(int,int)), this, SLOT(fuckOffSlot(int,int)) );
	connect(qmlWid->rootObject(), SIGNAL( startScanning() ), this, SLOT( startScanning()) );
}

SpinBoxWid::~SpinBoxWid() = default;

void SpinBoxWid::fuckOffSlot(int wid, int height)
{
	qInfo() << wid << "   " << height;
}

void SpinBoxWid::startScanning()
{
	qInfo() << "START SCANNING";
}



