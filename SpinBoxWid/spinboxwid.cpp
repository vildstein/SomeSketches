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
	m_startScanning = new QQuickWidget;
	m_startScanning->setAttribute(Qt::WA_DeleteOnClose);
	connect(m_startScanning, &QQuickWidget::destroyed, qApp, &QApplication::quit);

	m_startScanning->setResizeMode(QQuickWidget::SizeRootObjectToView);
	m_startScanning->setMinimumHeight(160);
	m_startScanning->setMinimumWidth(435);
	m_startScanning->resize(410, 160);

	QUrl file(("qrc:/wid.qml"));
	m_startScanning->setSource(file);

	//connect(qmlWid->rootObject(), SIGNAL(shitChanged(int,int)), this, SLOT(fuckOffSlot(int,int)));
	connect(m_startScanning->rootObject(), SIGNAL(widgetsVisibilityChanged(bool)), this, SLOT(onAngleInputVisChanged(bool)));
	connect(m_startScanning->rootObject(), SIGNAL(startScanning()), this, SLOT(startScanning()));

	lay->addWidget(m_startScanning);
	setLayout(lay);
	resize(sizeHint());
}

SpinBoxWid::~SpinBoxWid() = default;

void SpinBoxWid::startScanning()
{
	qInfo() << "START SCANNING";
}

void SpinBoxWid::onAngleInputVisChanged(bool isVisuble)
{
	if (isVisuble == true) {
		m_startScanning->setMinimumHeight(160);
	} else {
		m_startScanning->setMinimumHeight(80);
	}
}

