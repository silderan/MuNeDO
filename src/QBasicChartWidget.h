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

#include "Basic/QIniFile.h"

class WorkerThread : public QThread
{
	Q_OBJECT

	QVariant mResultData;
	QString mHostname;
	QString mID;

protected:
	void run() override;

public:
	WorkerThread(){}

	const QVariant &resultData() const	{ return mResultData;	}
	const QString &hostname() const		{ return mHostname;		}
	const QString &id() const			{ return mID;			}

	void setResultData(const QVariant &data)	{ mResultData = data;	}
	void setHostname(const QString &hostname)	{ mHostname = hostname;	}
	void setID(const QString &id)				{ mID = id;	}

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


struct QLineConfig
{
	QString mID;
	QString mLabel;
	QString mRemoteHost;
	QString mRequestType;
	QString mRequestValue;
	QStringList mRequestParameters;
	QColor mLineColor;
	QLineConfig(){}
	QLineConfig(const QLineConfig &other)
		: mID(other.mID)
		, mLabel(other.mLabel)
		, mRemoteHost(other.mRemoteHost)
		, mRequestType(other.mRequestType)
		, mRequestValue(other.mRequestValue)
		, mRequestParameters(other.mRequestParameters)
		, mLineColor(other.mLineColor)
	{	}
	QLineConfig(const QString &id,
						 const QString &label,
						 const QString &remoteHost,
						 const QString &requestType,
						 const QString &requestValue,
						 const QStringList &requestParameters,
						 const QColor &clr)
		: mID(id)
		, mLabel(label)
		, mRemoteHost(remoteHost)
		, mRequestType(requestType)
		, mRequestValue(requestValue)
		, mRequestParameters(requestParameters)
		, mLineColor(clr)
	{	}
	bool operator==(const QLineConfig &other)	{ return (mID == other.mID); }
	QLineConfig &operator=(const QLineConfig &other)
	{
		mID = other.mID;
		mLabel = other.mLabel;
		mRemoteHost = other.mRemoteHost;
		mRequestType = other.mRequestType;
		mRequestValue = other.mRequestValue;
		mRequestParameters = other.mRequestParameters;
		mLineColor = other.mLineColor;
		return *this;
	}
	bool load(const QString &preKey, const QIniData &data);
	void save(const QString &preKey, QIniData &iniData) const;
};

struct QChartConfig
{
	QString mChartType;
	QList<QLineConfig> mLines;
	bool load(const QIniData &data);
	QIniData &save(QIniData &data) const;
};

class QBasicChartLineConfigList : public QList<QLineConfig>
{
public:
	bool contains(const QString &label) const
	{
		for( const QLineConfig &bghl : *this )
			if( label == bghl.mID )
				return true;
		return false;
	}
	void append(const QLineConfig &t)
	{
		if( !contains(t.mID) )
			QList::append(t);
	}
};

struct QChartLine : public QLineConfig
{
	_qLineSeries *series;
	_qTimeAxis *axisX;
	_qValueAxis *axisY;
	QChartLine()
		: QLineConfig()
		, series(Q_NULLPTR)
		, axisX(Q_NULLPTR)
		, axisY(Q_NULLPTR)
	{	}
	QChartLine(const QChartLine &other)
		: QLineConfig(other)
		, series(other.series)
		, axisX(other.axisX)
		, axisY(other.axisY)
	{	}
	QChartLine(const QLineConfig &other)
		: QLineConfig(other)
		, series(Q_NULLPTR)
		, axisX(Q_NULLPTR)
		, axisY(Q_NULLPTR)
	{	}
	QLineConfig &operator=(const QLineConfig &other)
	{
		QLineConfig::operator=(other);
		return *this;
	}

	void changeColor(const QColor &clr)
	{
		QPen p(mLineColor = clr);
		p.setWidth(1);
		series->setPen(p);
	}
	void setLabel(const QString label)		{ series->setName(mLabel = label);	}

	bool isValid() const	{ return !mLabel.isEmpty();	}
};

class QChartLineList : public QList<QChartLine>
{
public:
	const QChartLine&operator[](const QString &id) const
	{
		for( const QChartLine &line : *this )
			if( line.mID == id )
				return line;
		static QChartLine dummy_line;
		return dummy_line;
	}
	QChartLine&operator[](const QString &id)
	{
		for( QChartLine &line : *this )
			if( line.mID == id )
				return line;
		static QChartLine dummy_line;
		Q_ASSERT(!dummy_line.isValid());
		return dummy_line;
	}
	bool contains(const QString &id) const
	{
		for( const QChartLine &line : *this )
			if( line.mID == id )
				return true;
		return false;
	}
	bool remove(const QString &id)
	{
		for( int i = count()-1; i >= 0; --i )
			if( at(i).mID == id )
			{
				removeAt(i);
				return true;
			}
		return false;
	}
};

class QBasicChart : public _qCharts
{
	QChartLineList lines;
	QString mChartType;
	QDateTime mInitialTime;	// Initial Time for x axis.
	QDateTime leftLimit;	// Right limit for x axis.
	QDateTime rightLimit;	// Right limit for x axis.

protected:
	void updateChartMaxAxis();
	void setValueRange(const qreal &min, const qreal &max);
	void setTimeRange(const qreal &minMSec, const qreal &maxMSec);
	void setTimeRange(const QDateTime &minTime, const QDateTime &maxTime);

public:
	explicit QBasicChart(const QString &chartType, QGraphicsItem *parent = Q_NULLPTR, Qt::WindowFlags wFlags = Qt::Widget);

	const QChartLineList &chartLines() const	{ return lines;	}
	int linesCount() const				{ return lines.count();	}
	const QString &chartType() const	{ return mChartType;	}

	QChartConfig getChartConfig() const;

	QChartLine &addLine(const QString &id,
				   const QString &label,
				   const QString &remoteHost,
				   const QString &requestType,
				   const QString &requestValue,
				   const QStringList &requestParameters,
				   const QColor &clr)
	{
		return addLine( QLineConfig(id, label, remoteHost, requestType, requestValue, requestParameters, clr) );
	}
	QChartLine &addLine(const QLineConfig &lineConfig);
	void delLine(const QLineConfig &lineConfig);

	void addValue(const QString &lineID, unsigned long value);
//	QBasicChartLineConfigList basicChartLineConfigList() const;
	void setInitialTime(const QDateTime &initialTime);

	// Set times to be shown in chart.
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

	WorkerThread *getFreeThread();

	QBasicChart *chart()				{ return mChart;	}
	const QBasicChart *chart() const	{ return mChart;	}

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

public:
	QBasicChartWidget(QTabChartHolder *chartHolder, const QString &chartType);

	QChartConfig getChartConfig() const			{ return chart()->getChartConfig();	}
	QTabChartHolder *chartHolder()				{ return mChartHolder;	}
	const QTabChartHolder *chartHolder() const	{ return mChartHolder;	}

	const QChartLineList &chartLines() const	{ return mChart->chartLines();	}
	int linesCount() const						{ return mChart->linesCount();	}

	void addHost(const QString &id, const QString &label, const QString &remoteHost,const QString &requestType,
				 const QString &requestValue,const QStringList &requestParameters, const QColor &clr)
	{
		mChart->addLine(id, label, remoteHost, requestType, requestValue, requestParameters, clr);
	}
	void addHost(const QLineConfig &lineConfig)				{ mChart->addLine(lineConfig);	}

	void delHost(const QLineConfig &lineConfig);
	void setInitialTime(const QDateTime &initialTime)			{ mChart->setInitialTime(initialTime);	}
	// Set times to be shown in chart.
	// If firstTime is invalid, it defaults to initialTime
	// If lastTime is invalid, it defaults to currentTime.
	void setTimeRange(const QDateTime &firstTime, const QDateTime &lastTime)	{ mChart->setTimes(firstTime, lastTime);	}

	virtual void addValue(const QString &id, unsigned long value)				{ return chart()->addValue(id, value);}
	virtual void on_ResultReady(WorkerThread *wt);
	virtual void on_DoJob(WorkerThread *wt) = 0;

	void heartbeat();

signals:
	void dobleClic(QBasicChartWidget *chartWidget);
	void rightClic(QBasicChartWidget *chartWidget);
};
#endif // QBASICCHARTWIDGET_H
