/**************************************************************************

  Copyright 2015-2020 Rafael Dellà Bort. silderan (at) gmail (dot) com

  This file is part of MuNeDO (Multiple Network Device Observer)

  MuNeDO is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  MuNeDO is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  and GNU Lesser General Public License. along with MuNeDO.
  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/
#ifndef QBASICCHARTWIDGET_H
#define QBASICCHARTWIDGET_H

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include <QMap>
#include <QValueAxis>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QThread>
#include <QList>


class WorkerThread : public QThread
{
	Q_OBJECT

	QVariant mResultData;
	QString mHostname;

protected:
	void run() override;

public:
	WorkerThread(){}

	const QVariant &resultData() const	{ return mResultData;	}
	const QString &hostname() const		{ return mHostname;		}

	void setResultData(const QVariant &data)	{ mResultData = data;	}
	void setHostname(const QString &hostname)	{mHostname = hostname;	}
signals:
	void resultReady(WorkerThread *thread);
	void doJob(WorkerThread *thread);
};

#ifdef QT_CHARTS_NAMESPACE
using _qCharts = QT_CHARTS_NAMESPACE::QChart;
using _qChartWidget = QT_CHARTS_NAMESPACE::QChartView;
using _qLineSeries = QT_CHARTS_NAMESPACE::QLineSeries;
using _qValueAxis = QT_CHARTS_NAMESPACE::QValueAxis;
using _qTimeAxis = QT_CHARTS_NAMESPACE::QDateTimeAxis;
#else
using _qCharts = QChart;
using _qChartWidget = QChartView;
using _qLineSeries = QLineSeries;
using _qValueAxis = QValueAxis;
using _qTimeAxis = QDateTimeAxis;
#endif


struct BasicGraphLineConfig
{
	QString mRemoteHost;
	QColor mLineColor;
	BasicGraphLineConfig(){}
	BasicGraphLineConfig(const BasicGraphLineConfig &other)
		: mRemoteHost(other.mRemoteHost)
		, mLineColor(other.mLineColor)
	{	}
	BasicGraphLineConfig(const QString &remoteHost, const QColor &clr)
		: mRemoteHost(remoteHost)
		, mLineColor(clr)
	{	}
	bool operator==(const BasicGraphLineConfig &other)
	{
		return (mRemoteHost == other.mRemoteHost) && (mLineColor == other.mLineColor);
	}
};
class QBasicGraphLineConfigList : public QList<BasicGraphLineConfig>
{
public:
	bool contains(const QString &hostname) const
	{
		for( const BasicGraphLineConfig &bghl : *this )
			if( hostname == bghl.mRemoteHost )
				return true;
		return false;
	}
	void append(const BasicGraphLineConfig &t)
	{
		if( !contains(t.mRemoteHost) )
			QList::append(t);
	}
};

class QBasicChart : public _qCharts
{
	struct _line
	{
		_qLineSeries *series;
		_qTimeAxis *axisX;
		_qValueAxis *axisY;
		BasicGraphLineConfig mBasicGraphLineConfig;
		_line()
			: series(new _qLineSeries())
			, axisX(new _qTimeAxis())
			, axisY(new _qValueAxis())
		{
			axisX->setTickCount(1);
			axisX->setFormat("dd/MM/yy hh:mm:ss");
			axisY->setLabelFormat("%ims");
		}
		_line(const _line &other)
			: series(other.series)
			, axisX(other.axisX)
			, axisY(other.axisY)
			, mBasicGraphLineConfig(other.mBasicGraphLineConfig)
		{	}

		void changeColor(const QColor &clr)
		{
			QPen p(mBasicGraphLineConfig.mLineColor = clr);
			p.setWidth(1);
			series->setPen(p);
		}
	};
	QMap<QString, _line> lines;

	QDateTime mInitialTime;	// Initial Time for x axis.
	QDateTime leftLimit;	// Right limit for x axis.
	QDateTime rightLimit;	// Right limit for x axis.

protected:
	void updateChartMaxAxis();
	void setValueRange(const qreal &min, const qreal &max);
	void setTimeRange(const qreal &minMSec, const qreal &maxMSec);
	void setTimeRange(const QDateTime &minTime, const QDateTime &maxTime);

public:
	explicit QBasicChart(QGraphicsItem *parent = Q_NULLPTR, Qt::WindowFlags wFlags = Qt::Widget);
	_line &addLine(const QString &hostname, const QColor &clr);
	_line &addLine(const BasicGraphLineConfig &bglc)
	{
		return addLine(bglc.mRemoteHost, bglc.mLineColor);
	}
	void delLine(const BasicGraphLineConfig &bglc);

	void addValue(const QString &hostname, unsigned long value);
	QBasicGraphLineConfigList basicGraphLineConfigList();
	void setInitialTime(const QDateTime &initialTime);

	// Set times to be shown in graph.
	// If firstTime is invalid, it defaults to initialTime
	// If lastTime is invalid, it defaults to currentTime.
	void setTimes(const QDateTime &firstTime, const QDateTime &lastTime);
};

class QTabChartHolder;

class QBasicChartWidget : public _qChartWidget
{
Q_OBJECT

	QBasicChart *mChart;
	QList<WorkerThread*> mAllThreads;
	QTabChartHolder *mChartHolder;
	QBasicGraphLineConfigList mGraphicLineConfigList;

	WorkerThread *getFreeThread();

	QBasicChart *chart()				{ return mChart;	}
	const QBasicChart *chart() const	{ return mChart;	}

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

public:
	QBasicChartWidget(QTabChartHolder *chartHolder);

	QBasicGraphLineConfigList basicGraphLineConfigList()	{ return chart()->basicGraphLineConfigList();	}
	QTabChartHolder *chartHolder()				{ return mChartHolder;	}
	const QTabChartHolder *chartHolder() const	{ return mChartHolder;	}

	void addHost(const QString &hostname, const QColor &clr);
	void addHost(const BasicGraphLineConfig &bglc)				{ addHost(bglc.mRemoteHost, bglc.mLineColor);	}
	void delHost(const BasicGraphLineConfig &bglc);
	void setInitialTime(const QDateTime &initialTime)			{ mChart->setInitialTime(initialTime);	}
	// Set times to be shown in graph.
	// If firstTime is invalid, it defaults to initialTime
	// If lastTime is invalid, it defaults to currentTime.
	void setTimes(const QDateTime &firstTime, const QDateTime &lastTime)	{ mChart->setTimes(firstTime, lastTime);	}

	virtual void addValue(const QString &hostname, unsigned long value)	{ return chart()->addValue(hostname, value);}
	virtual void on_ResultReady(WorkerThread *wt);
	virtual void on_DoJob(WorkerThread *wt) = 0;

	void heartbeat();

signals:
	void dobleClic(QBasicChartWidget *chartWidget);
	void rightClic(QBasicChartWidget *chartWidget);
};
#endif // QBASICCHARTWIDGET_H
