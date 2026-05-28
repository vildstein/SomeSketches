#pragma once

#include <QFrame>
#include <QRandomGenerator>

class QSpinBox;
class QDial;
class QTimer;
class QVBoxLayout;

class RaRssiSnrEmuWidget : public QFrame
{
	Q_OBJECT

public:
	explicit RaRssiSnrEmuWidget(QWidget* parent = nullptr);
	virtual ~RaRssiSnrEmuWidget();

	void setCurrentDirectionAngle(const QPoint& pos);

signals:
	void proceedRssiSignal(qint32 rssi, qint32 snr);

//protected:
	//virtual void mousePressEvent(QMouseEvent *event) override;

private:
	void onDialValueChanged(int val);
	void onSpinBoxOfDialValueChanged(int val);
	void startOrStopRssiTimer(bool checked);
	void randomazeRSSiAndSnr();
	void onTimerSpinBoxValueChanged(int val);

	QVBoxLayout* createRssiAndSnrConfigLayout();

private:
	//QSpinBox* m_minSB{nullptr};
	//QSpinBox* m_maxSB{nullptr};
	//QSpinBox* m_intervalOfAngleSB{nullptr};

	QSpinBox* m_spinBoxOfDial{nullptr};
	QSpinBox* m_timerSB{nullptr};
	QDial* m_directionDial{nullptr};
	QTimer* m_rssiTimer;
	QRandomGenerator m_rssiRandomizer;
	QRandomGenerator m_snrRandomizer;

	double m_angle{0.0};
	double m_azimuth{0.0};

	double m_angleBottomOffset{0.0};
	double m_angleTopOffset{0.0};

	static constexpr const double m_angleDelta{2.0};
	// For RandomGen
	qint32 m_rssOnBoardMin{-22};
	qint32 m_rssOnBoardMax{-15};

	qint32 m_snrOnBoardMin{15};
	qint32 m_snrOnBoardMax{22};

	qint32 m_rssWithoutBoardMin{-45};
	qint32 m_rssWithoutBoardMax{-28};

	qint32 m_snrWithoutBoardMin{3};
	qint32 m_snrWithoutBoardMax{12};

	// For RandomGen
};
