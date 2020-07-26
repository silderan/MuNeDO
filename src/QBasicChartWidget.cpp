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
#include "QBasicChartWidget.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

#include "QTabChartHolder.h"

void WorkerThread::run()
{
	emit doJob(this);
	emit resultReady(this);
}

QT_CHARTS_USE_NAMESPACE

bool QLineConfig::load(const QString &preKey, const QIniData &iniData)
{
	if( iniData.contains(QString("%1id").arg(preKey)) )
	{
		QStringList colors = iniData[QString("%1color").arg(preKey)].split(":");
		if( colors.count() == 3 )
		{
			mID					= iniData[QString("%1id").arg(preKey)];
			mLabel				= iniData[QString("%1label").arg(preKey)];
			mRemoteHost			= iniData[QString("%1remote").arg(preKey)];
			mRequestType		= iniData[QString("%1type").arg(preKey)];
			mRequestValue		= iniData[QString("%1value").arg(preKey)];
			mRequestParameters	= iniData[QString("%1params").arg(preKey)].split(",");
			mLineColor			= QColor(colors[0].toInt(),		// Red
										 colors[1].toInt(),		// Green
										 colors[2].toInt());	// Blue
			return true;
		}
	}
	return false;
}

void QLineConfig::save(const QString &preKey, QIniData &iniData) const
{
	iniData[QString("%1id").arg(preKey)]		= mID;
	iniData[QString("%1label").arg(preKey)]		= mLabel;
	iniData[QString("%1remote").arg(preKey)]	= mRemoteHost;
	iniData[QString("%1type").arg(preKey)]		= mRequestType;
	iniData[QString("%1value").arg(preKey)]		= mRequestValue;
	iniData[QString("%1params").arg(preKey)]	= mRequestParameters.join(",");
	iniData[QString("%1color").arg(preKey)]		= QString("%1:%2:%3").arg(mLineColor.red())
																				 .arg(mLineColor.green())
																				 .arg(mLineColor.blue());
}

bool QChartConfig::load(const QIniData &data)
{
	mChartType = data["chartType"];
	QLineConfig lineConfig;
	for( int lineID = 0 ; lineConfig.load(QString("line_%1_").arg(lineID), data); ++lineID )
		mLines.append(lineConfig);
	return true;
}

QIniData &QChartConfig::save(QIniData &data) const
{
	data["chartType"] = mChartType;
	int lineID = 0;
	for( const QLineConfig &lineConfig : mLines )
		lineConfig.save(QString("line_%1_").arg(lineID++), data);
	return data;
}

QBasicChart::QBasicChart(const QString &chartType, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
	, mChartType(chartType)
{
	setMargins( QMargins() );
}

QChartConfig QBasicChart::getChartConfig() const
{
	QChartConfig chartConfig;

	chartConfig.mChartType = mChartType;
	for( const QChartLine &line : lines )
		chartConfig.mLines.append(line);

	return chartConfig;
}

QChartLine &QBasicChart::addLine(const QLineConfig &lineConfig)
{
	QChartLine &line = lines[lineConfig.mID];
	if( line.isValid() )
	{
		line.changeColor(lineConfig.mLineColor);
		line.setLabel(lineConfig.mLabel);
		return line;
	}
	else
	{
		lines.append(QChartLine(lineConfig));
		QChartLine &line = lines.last();

		line = lineConfig;
		line.series = new _qLineSeries();
		line.axisX = new _qTimeAxis();
		line.axisY = new _qValueAxis();

		line.axisX->setTickCount(1);
		line.axisX->setFormat("dd/MM/yy hh:mm:ss");
		line.axisY->setLabelFormat("%ims");

		addSeries(line.series);

		addAxis(line.axisX, Qt::AlignBottom);
		addAxis(line.axisY, Qt::AlignLeft);

		line.series->attachAxis(line.axisX);
		line.series->attachAxis(line.axisY);

		line.setLabel(line.mLabel);
		line.changeColor(line.mLineColor);

		line.axisX->setRange( leftLimit.isValid() ? leftLimit : mInitialTime.isValid() ? mInitialTime : QDateTime::currentDateTime(),
							  rightLimit.isValid() ? rightLimit : QDateTime::currentDateTime().addMSecs(1) );

		// If it's the first line, it must be visible. The other ones muy be hiden.
		if( lines.count() != 1 )
		{
			line.axisY->hide();
			line.axisX->hide();
		}
		return line;
	}
}

void QBasicChart::delLine(const QLineConfig &lineConfig)
{
	QChartLine &line = lines[lineConfig.mID];
	removeAxis( line.axisX );
	removeAxis( line.axisY );
	removeSeries( line.series );

	line.axisX->deleteLater();	line.axisX = Q_NULLPTR;
	line.axisY->deleteLater();	line.axisY = Q_NULLPTR;
	line.series->deleteLater();	line.series = Q_NULLPTR;

	lines.remove( lineConfig.mID );

	// If line is visible, as it's gona be removed, visibility goes to another one.
	if( lines.count() && !lines.first().axisX->isVisible() )
	{
		lines.first().axisX->show();
		lines.first().axisY->show();
	}
	updateChartMaxAxis();
}

// Called when time axis is modified or when a line is removed.
// This updates Y axis values acordigly to the visible values.
void QBasicChart::updateChartMaxAxis()
{
	qreal newMax = 0;
	qreal minTime = leftLimit.isValid() ? leftLimit.toMSecsSinceEpoch() : 0;
	qreal maxTime = rightLimit.isValid() ? rightLimit.toMSecsSinceEpoch() : QDateTime::currentMSecsSinceEpoch();
	qreal curTime;
	for( const QChartLine &line : lines )
	{
		for( int i = line.series->count()-1; i>=0; --i )
		{
			curTime = line.series->at(i).x();
			if( (curTime >= minTime) && (curTime <= maxTime) )
			{
				if( newMax < line.series->at(i).y() )
					newMax = line.series->at(i).y();
			}
		}
	}
	setValueRange(0, newMax);
}

void QBasicChart::setValueRange(const qreal &min, const qreal &max)
{
	for( QChartLine &line : lines )
		line.axisY->setRange(min, max);
}

void QBasicChart::setTimeRange(const qreal &minMSec, const qreal &maxMSec)
{
	setTimeRange( QDateTime::fromMSecsSinceEpoch(qint64(minMSec)), QDateTime::fromMSecsSinceEpoch(qint64(maxMSec)) );
}

void QBasicChart::setTimeRange(const QDateTime &minTime, const QDateTime &maxTime)
{
	for( QChartLine &line : lines )
		line.axisX->setRange( minTime, maxTime );
}

void QBasicChart::addValue(const QString &lineID, unsigned long value)
{
	if( !lines.contains(lineID) )
	{
		qDebug() << "Ninguna linea con ID " << lineID <<". Quizá se haya borrado.";
		return;
	}

//	qDebug() << "Adding in " << hostname << ": " << value << " at " << QDateTime::currentDateTime();
	lines[lineID].series->append(QDateTime::currentMSecsSinceEpoch(), value);

	for( QChartLine &line : lines )
	{
		if( value > line.axisY->max() )
			line.axisY->setMax(value);
		if( !rightLimit.isValid() )
			line.axisX->setMax(QDateTime::currentDateTime());
	}
}

void QBasicChart::setInitialTime(const QDateTime &initialTime)
{
	mInitialTime = initialTime;
	for( QChartLine &line : lines )
		line.axisX->setMin(initialTime);
}

void QBasicChart::setTimes(const QDateTime &firstTime, const QDateTime &lastTime)
{
	leftLimit = firstTime.isValid() ? firstTime : mInitialTime;
	rightLimit = lastTime;

	for( QChartLine &line : lines )
		line.axisX->setRange( leftLimit, rightLimit.isValid() ? rightLimit : QDateTime::currentDateTime() );
}

QBasicChartWidget::QBasicChartWidget(QTabChartHolder *chartHolder, const QString &chartType)
	: _qChartWidget(mChart = new QBasicChart(chartType))
	, mChartHolder(chartHolder)
{
}

WorkerThread *QBasicChartWidget::getFreeThread()
{
	for( WorkerThread *hilo : mAllThreads )
		if( hilo->isFinished() )
			return hilo;

	WorkerThread *workerThread = new WorkerThread();
	mAllThreads.append(workerThread);
	connect(workerThread, &WorkerThread::doJob, this, &QBasicChartWidget::on_DoJob);
	connect(workerThread, &WorkerThread::resultReady, this, &QBasicChartWidget::on_ResultReady);

	return workerThread;
}

void QBasicChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit dobleClic(this);
}

void QBasicChartWidget::heartbeat()
{
	for( const QChartLine &line : chartLines() )
	{
		WorkerThread *wt = getFreeThread();
		wt->setHostname( line.mRemoteHost );
		wt->setID( line.mID );
		wt->start();
	}
}

void QBasicChartWidget::delHost(const QLineConfig &lineConfig)
{
	chart()->delLine(lineConfig);
}

void QBasicChartWidget::on_ResultReady(WorkerThread *wt)
{
	addValue( wt->id(), wt->resultData().toUInt() );
}

