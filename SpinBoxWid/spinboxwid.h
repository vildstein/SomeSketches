#ifndef SPINBOXWID_H
#define SPINBOXWID_H

#include <QWidget>

class SpinBoxWid : public QWidget
{
	Q_OBJECT

public:
	explicit SpinBoxWid(QWidget *parent = nullptr);
	virtual ~SpinBoxWid();
};
#endif // SPINBOXWID_H
