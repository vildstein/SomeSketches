#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QDial>
#include <QCheckBox>
#include <QTimer>
#include <QValidator>
#include <QDoubleValidator>

#include <QMouseEvent>

#include "RaRssiSnrEmuWidget.h"

RaRssiSnrEmuWidget::RaRssiSnrEmuWidget(QWidget* parent) :
	QFrame(parent), m_rssiTimer(new QTimer(this))
	//m_intervalLastValue(0)
{
	m_directionDial = new QDial;
	m_directionDial->setNotchesVisible(false);
	m_directionDial->setWrapping(true);
	m_directionDial->setRange(0, 359);
	m_directionDial->setSingleStep(1);
	m_directionDial->setOrientation(Qt::Horizontal);
	m_directionDial->setValue(180);

	m_spinBoxOfDial = new QSpinBox;
	m_spinBoxOfDial->setRange(0, 359);
	m_spinBoxOfDial->setSuffix(" гр");
	auto dialLay = new QGridLayout;
	m_azForSignalLab = new QLabel(tr("Сигнал отправляется на азимут: "));
	dialLay->addWidget(m_azForSignalLab, 0, 0, Qt::AlignCenter);
	m_azLabel = new QLabel(tr("Борт находится по азимуту: "));
	dialLay->addWidget(m_azLabel, 1, 0, Qt::AlignHCenter | Qt::AlignTop);
	dialLay->addWidget(m_spinBoxOfDial, 2, 0, Qt::AlignHCenter | Qt::AlignTop);
	m_directionDial->setLayout(dialLay);

	m_timerSB = new QSpinBox;
	m_timerSB->setSuffix(" сек");
	m_timerSB->setRange(1, 300);
	m_timerSB->setValue(1);

	auto timerChBox = new QCheckBox(tr("Таймер Активен"));
	connect(m_timerSB, QOverload<int>::of(&QSpinBox::valueChanged), this, &RaRssiSnrEmuWidget::onSpinBoxOfDialValueChanged);

	auto dialLayout = new QVBoxLayout;
	auto timerChBoxLay = new QHBoxLayout;
	timerChBoxLay->addWidget(m_timerSB);
	timerChBoxLay->addWidget(timerChBox);

	dialLayout->addWidget(m_directionDial, Qt::AlignCenter);
	dialLayout->addLayout(timerChBoxLay);

	auto rssSnrLay = createRssiAndSnrConfigLayout();
	dialLayout->addLayout(rssSnrLay);

	setAttribute(Qt::WA_DeleteOnClose);
	setLayout(dialLayout);

	setMinimumSize(350, 750);
	resize(400, 800);

	connect(m_directionDial, &QDial::valueChanged, this, &RaRssiSnrEmuWidget::onDialValueChanged);
	connect(m_spinBoxOfDial, QOverload<int>::of(&QSpinBox::valueChanged), this, &RaRssiSnrEmuWidget::onSpinBoxOfDialValueChanged);

	connect(timerChBox, &QCheckBox::toggled, this, &RaRssiSnrEmuWidget::startOrStopRssiTimer);
	connect(m_rssiTimer, &QTimer::timeout, this, &RaRssiSnrEmuWidget::randomazeRSSiAndSnr);

	m_spinBoxOfDial->setValue(24);
}

// void RaRssiSnrEmuWidget::mousePressEvent(QMouseEvent* event)
// {
// 	//event->ignore();

// 	delete event;

// }

#include <QDebug>

// void RaRssiSnrEmuWidget::setCurrentDirectionAngle(const QPoint& incomePositon)
// {
// 	//Q_UNUSED(uavComplexId);
// 	//auto raDirectionStruct  = UavComplexNS::raDirectionFromByteArray(ba);

// 	auto thisPos = pos();
// 	auto angle = QLineF(thisPos, incomePositon).angle();

// 	qInfo() << angle;

// 	m_azimuth = angle;
// 	m_angle = angle;
// }

RaRssiSnrEmuWidget::~RaRssiSnrEmuWidget() = default;

void RaRssiSnrEmuWidget::onDialValueChanged(int val)
{
	int summ{0};
	{
		//QSignalBlocker sb(m_spinBoxOfDial);
		val >= 180 ? (summ = val - 180) : (summ = 180 + val);
		//m_spinBoxOfDial->setValue(summ);
		QString text("Сигнал отправляется на азимут: ");
		m_azForSignalLab->setText(QString(text + QString::number(summ)));
		emit azimuthChanged(summ);
	}

	m_angleBottomOffset = static_cast<double>(summ);
	m_angleTopOffset = static_cast<double>(summ);

	m_angleBottomOffset -= m_angleDelta;
	m_angleTopOffset += m_angleDelta;
}

void RaRssiSnrEmuWidget::onSpinBoxOfDialValueChanged(int val)
{

	m_azimuth = static_cast<double>(val);
	m_azLabel->clear();
	QString text("Борт находится по азимуту: ");
	m_azLabel->setText(QString(text + QString::number(m_azimuth)));
	emit boardAzChanged(m_azimuth);


	// int summ{0};
	// {
	// 	QSignalBlocker sb(m_directionDial);
	// 	val > 180 ? (summ = val - 180) : (summ = 180 + val);
	// 	m_directionDial->setValue(summ);
	// 	emit azimuthChanged(summ);
	// }

	// m_angleBottomOffset = static_cast<double>(val);
	// m_angleTopOffset = static_cast<double>(val);

	// m_angleBottomOffset -= m_angleDelta;
	// m_angleTopOffset += m_angleDelta;
}

void RaRssiSnrEmuWidget::startOrStopRssiTimer(bool checked)
{
	if (checked == true) {
		auto timerInterval = (m_timerSB->value()) * 1000;
		m_rssiTimer->setInterval(timerInterval);
		m_rssiTimer->start();
	} else {
		m_rssiTimer->stop();
	}
}

#include <QDebug>

void RaRssiSnrEmuWidget::randomazeRSSiAndSnr()
{
	qint32 rssi{0};
	qint32 snr{0};

	if (m_azimuth > m_angleBottomOffset && m_azimuth < m_angleTopOffset) {
		rssi = m_rssiRandomizer.bounded(m_rssOnBoardMin, m_rssOnBoardMax);
		snr = m_snrRandomizer.bounded(m_snrOnBoardMin, m_snrOnBoardMax);
	} else {
		rssi = m_rssiRandomizer.bounded(m_rssWithoutBoardMin, m_rssWithoutBoardMax);
		snr = m_snrRandomizer.bounded(m_snrWithoutBoardMin, m_snrWithoutBoardMax);
	}
	emit proceedRssiSignal(rssi, snr);
}

void RaRssiSnrEmuWidget::onTimerSpinBoxValueChanged(int val)
{
	if (m_rssiTimer->isActive()) {
		m_rssiTimer->stop();
		auto timerInterval = val * 1000;
		m_rssiTimer->setInterval(timerInterval);
		m_rssiTimer->start();
	}
}

QVBoxLayout* RaRssiSnrEmuWidget::createRssiAndSnrConfigLayout()
{
	auto createLine = []()->QLabel*{
			auto lineLabel = new QLabel;
			lineLabel->setFrameStyle(QFrame::HLine | QFrame::Plain);
			lineLabel->setLineWidth(4);
			return lineLabel;
	};


	auto minRssiOnBoardLay = new QHBoxLayout;
	auto rssiMinOnBoardLabel = new QLabel(tr("Мин RSSI с борта"));
	auto minRSSISB = new QSpinBox;
	minRSSISB->setRange(-100, 0);
	minRSSISB->setValue(m_rssOnBoardMin);
	minRssiOnBoardLay->addWidget(rssiMinOnBoardLabel);
	minRssiOnBoardLay->addWidget(minRSSISB);

	auto maxRssiOnBoardLay = new QHBoxLayout;
	auto rssiMaxOnBoardLabel = new QLabel(tr("Макс RSSI с борта"));
	auto maxRSSISB = new QSpinBox;
	maxRSSISB->setRange(-100, 0);
	maxRSSISB->setValue(m_rssOnBoardMax);
	maxRssiOnBoardLay->addWidget(rssiMaxOnBoardLabel);
	maxRssiOnBoardLay->addWidget(maxRSSISB);

	auto lessLam = [](int val, QSpinBox* maxSP, QSpinBox* minSB, qint32 max, qint32& min) {
		if (val < max && val < maxSP->value()) {
			min = val;
		} else {
			val = min;
			minSB->setValue(val);
		}
	};
	auto moreLam = [](int val, QSpinBox* maxSP, QSpinBox* minSB, qint32& max, qint32 min){
		if (val > min && val > minSB->value()) {
			max = val;
		} else {
			val = max;
			maxSP->setValue(val);
		};
	};

	connect(minRSSISB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxRSSISB, minRSSISB, lessLam](int val){
		lessLam(val, maxRSSISB, minRSSISB, m_rssOnBoardMax, m_rssOnBoardMin);
	});
	connect(maxRSSISB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxRSSISB, minRSSISB, moreLam](int val){
		moreLam(val, maxRSSISB, minRSSISB, m_rssOnBoardMax, m_rssOnBoardMin);
	});

	auto minSNROnBoardLay = new QHBoxLayout;
	auto minSNROnBoardLabel = new QLabel(tr("Мин SNR с борта"));
	auto minSNRSB = new QSpinBox;
	minSNRSB->setRange(0, 100);
	minSNRSB->setValue(m_snrOnBoardMin);
	minSNROnBoardLay->addWidget(minSNROnBoardLabel);
	minSNROnBoardLay->addWidget(minSNRSB);

	auto maxSNROnBoardLay = new QHBoxLayout;
	auto maxSNROnBoardLabel = new QLabel(tr("Макс SNR с борта"));
	auto maxSNRSB = new QSpinBox;
	maxSNRSB->setRange(0, 100);
	maxSNRSB->setValue(m_snrOnBoardMax);
	maxSNROnBoardLay->addWidget(maxSNROnBoardLabel);
	maxSNROnBoardLay->addWidget(maxSNRSB);

	connect(minSNRSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxSNRSB, minSNRSB, lessLam](int val){
		lessLam(val, maxSNRSB, minSNRSB, m_snrOnBoardMax, m_snrOnBoardMin);
	});
	connect(maxSNRSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxSNRSB, minSNRSB, moreLam](int val){
		moreLam(val, maxSNRSB, minSNRSB, m_snrOnBoardMax, m_snrOnBoardMin);
	});

	auto minRssiOutBoardLay = new QHBoxLayout;
	auto rssiMinOutBoardLabel = new QLabel(tr("Мин RSSI вне борта"));
	auto minRSSIOutBoardSB = new QSpinBox;
	minRSSIOutBoardSB->setRange(-100, 0);
	minRSSIOutBoardSB->setValue(m_rssWithoutBoardMin);
	minRssiOutBoardLay->addWidget(rssiMinOutBoardLabel);
	minRssiOutBoardLay->addWidget(minRSSIOutBoardSB);

	auto maxRssiOutBoardLay = new QHBoxLayout;
	auto rssiMaxOutBoardLabel = new QLabel(tr("Макс RSSI вне борта"));
	auto maxRSSIOutBoardSB = new QSpinBox;
	maxRSSIOutBoardSB->setRange(-100, 0);
	maxRSSIOutBoardSB->setValue(m_rssWithoutBoardMax);
	maxRssiOutBoardLay->addWidget(rssiMaxOutBoardLabel);
	maxRssiOutBoardLay->addWidget(maxRSSIOutBoardSB);

	connect(minRSSIOutBoardSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxRSSIOutBoardSB, minRSSIOutBoardSB, lessLam](int val){
		lessLam(val, maxRSSIOutBoardSB, minRSSIOutBoardSB, m_rssWithoutBoardMax, m_rssWithoutBoardMin);
	});
	connect(maxRSSIOutBoardSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxRSSIOutBoardSB, minRSSIOutBoardSB, moreLam](int val){
		moreLam(val, maxRSSIOutBoardSB, minRSSIOutBoardSB, m_rssWithoutBoardMax, m_rssWithoutBoardMin);
	});

	auto minSNROutBoardLay = new QHBoxLayout;
	auto minSNROutBoardLabel = new QLabel(tr("Мин SNR вне борта"));
	auto minSNROutBoardSB = new QSpinBox;
	minSNROutBoardSB->setRange(0, 100);
	minSNROutBoardSB->setValue(m_snrWithoutBoardMin);
	minSNROutBoardLay->addWidget(minSNROutBoardLabel);
	minSNROutBoardLay->addWidget(minSNROutBoardSB);

	auto maxSNROutBoardLay = new QHBoxLayout;
	auto maxSNROutBoardLabel = new QLabel(tr("Макс SNR вне борта"));
	auto maxSNROutBoardSB = new QSpinBox;
	maxSNROutBoardSB->setRange(0, 100);
	maxSNROutBoardSB->setValue(m_snrWithoutBoardMax);
	maxSNROutBoardLay->addWidget(maxSNROutBoardLabel);
	maxSNROutBoardLay->addWidget(maxSNROutBoardSB);

	connect(minSNROutBoardSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxSNROutBoardSB, minSNROutBoardSB, lessLam](int val){
		lessLam(val, maxSNROutBoardSB, minSNROutBoardSB, m_snrWithoutBoardMax, m_snrWithoutBoardMin);
	});
	connect(maxSNROutBoardSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, maxSNROutBoardSB, minSNROutBoardSB, moreLam](int val){
		moreLam(val, maxSNROutBoardSB, minSNROutBoardSB, m_snrWithoutBoardMax, m_snrWithoutBoardMin);
	});


	auto mainVertLay = new QVBoxLayout;
	mainVertLay->addWidget(createLine());
	mainVertLay->addLayout(minRssiOnBoardLay);
	mainVertLay->addLayout(maxRssiOnBoardLay);
	mainVertLay->addLayout(minSNROnBoardLay);
	mainVertLay->addLayout(maxSNROnBoardLay);
	mainVertLay->addWidget(createLine());
	mainVertLay->addLayout(minRssiOutBoardLay);
	mainVertLay->addLayout(maxRssiOutBoardLay);
	mainVertLay->addLayout(minSNROutBoardLay);
	mainVertLay->addLayout(maxSNROutBoardLay);

	return mainVertLay;
}
