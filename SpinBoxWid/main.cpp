#include "spinboxwid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SpinBoxWid w;
	w.show();
	return a.exec();
}
