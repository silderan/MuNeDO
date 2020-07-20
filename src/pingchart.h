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

#ifndef QPINGCHART_H
#define QPINGCHART_H

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
};
using QBasicGraphLineConfigList = QList<BasicGraphLineConfig>;

class QBasicChart : public _qCharts
{
	Q_OBJECT

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

	QDateTime leftLimit;	// Right limit for x axis.
	QDateTime rightLimit;	// Right limit for x axis.
public:
	explicit QBasicChart(QGraphicsItem *parent = Q_NULLPTR, Qt::WindowFlags wFlags = Qt::Widget);
	_line &addLine(const QString &hostname, const QColor &clr);
	_line &addLine(const BasicGraphLineConfig &bglc)
	{
		return addLine(bglc.mRemoteHost, bglc.mLineColor);
	}

	void addValue(const QString &hostname, unsigned long value);
	QBasicGraphLineConfigList basicGraphLineConfigList();
};

class QTabGraphHolder;

class QChartWidget : public _qChartWidget
{
	QBasicChart *mChart;

protected:
	QList<WorkerThread*> mAllThreads;
	QBasicChart *chart()				{ return mChart;	}
	const QBasicChart *chart() const	{ return mChart;	}
	QTabGraphHolder *mGraphHolder;
	QBasicGraphLineConfigList mGraphicLineConfigList;

	WorkerThread *getFreeThread();
	void showContextMenu(const QPoint &pos);
public:
	QChartWidget(QTabGraphHolder *graphHolder)
		: _qChartWidget(mChart = new QBasicChart)
		, mGraphHolder(graphHolder)
	{
		setContextMenuPolicy(Qt::CustomContextMenu);

		connect(this, &QChartWidget::customContextMenuRequested, this, &QChartWidget::showContextMenu);
	}
	void heartbeat();
	virtual void editGraph() = 0;

	void addHost(const QString &hostname, const QColor &clr);

	virtual void on_DoJob(WorkerThread *wt) = 0;
	virtual void on_ResultReady(WorkerThread *wt) = 0;
};

class QPingChartWidget : public QChartWidget
{
public:
	QPingChartWidget(QTabGraphHolder *graphHolder)
		: QChartWidget(graphHolder)
	{	}
	virtual void editGraph() override;
	virtual void on_DoJob(WorkerThread *wt) override;
	virtual void on_ResultReady(WorkerThread *wt) override;
};

#endif // QPINGCHART_H
