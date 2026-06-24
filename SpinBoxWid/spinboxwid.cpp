#include "spinboxwid.h"

#include <QQuickWidget>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QApplication>

#include <QDebug>

SpinBoxWid::SpinBoxWid(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* lay = new QVBoxLayout;
	qmlWid = new QQuickWidget;
	qmlWid->setAttribute(Qt::WA_DeleteOnClose);
	connect(qmlWid, &QQuickWidget::destroyed, qApp, &QApplication::quit);

	qmlWid->setResizeMode(QQuickWidget::SizeRootObjectToView);
	qmlWid->setMinimumHeight(160);
	qmlWid->setMinimumWidth(435);
	qmlWid->resize(410, 160);

	QUrl file(("qrc:/wid.qml"));
	qmlWid->setSource(file);

	lay->addWidget(qmlWid);
	setLayout(lay);

	resize(sizeHint());

	//connect(qmlWid->rootObject(), SIGNAL(shitChanged(int,int)), this, SLOT(fuckOffSlot(int,int)));
	connect(qmlWid->rootObject(), SIGNAL(widgetsVisibilityChanged(bool)), this, SLOT(onAngleInputVisChanged(bool)));
	connect(qmlWid->rootObject(), SIGNAL(startScanning()), this, SLOT(startScanning()));
}

SpinBoxWid::~SpinBoxWid() = default;

void SpinBoxWid::startScanning()
{
	qInfo() << "START SCANNING";
}

void SpinBoxWid::onAngleInputVisChanged(bool isVisuble)
{
	if (isVisuble == true) {
		qmlWid->setMinimumHeight(160);
	} else {
		qmlWid->setMinimumHeight(80);
	}
}

