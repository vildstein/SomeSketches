#include "spinboxwid.h"

#include <QQuickWidget>
#include <QVBoxLayout>

SpinBoxWid::SpinBoxWid(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* lay = new QVBoxLayout;
	QQuickWidget* qmlWid = new QQuickWidget;

	qmlWid->resize(400, 200);
	qmlWid->setResizeMode(QQuickWidget::SizeRootObjectToView);

	QUrl file(("qrc:/wid.qml"));
	qmlWid->setSource(file);

	lay->addWidget(qmlWid);
	setLayout(lay);

	resize(sizeHint());
}

SpinBoxWid::~SpinBoxWid() = default;

