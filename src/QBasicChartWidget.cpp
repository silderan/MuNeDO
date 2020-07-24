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

QBasicChart::QBasicChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
	setMargins( QMargins() );
}

QBasicChart::_line &QBasicChart::addLine(const QString &hostname, const QColor &clr)
{
	if( lines.contains(hostname) )
	{
		_line &line = lines[hostname];
		line.changeColor(clr);
		return line;
	}
	lines.insert(hostname, _line());
	_line &line = lines[hostname];

	line.mBasicGraphLineConfig.mRemoteHost = hostname;
	line.mBasicGraphLineConfig.mLineColor = clr;

	addSeries(line.series);

	addAxis(line.axisX, Qt::AlignBottom);
	addAxis(line.axisY, Qt::AlignLeft);

	line.series->attachAxis(line.axisX);
	line.series->attachAxis(line.axisY);
	line.series->setName(hostname);

	line.changeColor(clr);

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

void QBasicChart::delLine(const BasicGraphLineConfig &bglc)
{
	_line line = lines[bglc.mRemoteHost];
	removeAxis( line.axisX );
	removeAxis( line.axisY );
	removeSeries( line.series );
	lines.remove( bglc.mRemoteHost );

	// If line is visible, as it's gona be removed, visibility goes to another one.
	if( line.axisX->isVisible() && lines.count() )
	{
		lines.first().axisX->show();
		lines.first().axisY->show();
	}
	line.axisX->deleteLater();
	line.axisX = Q_NULLPTR;
	line.axisY->deleteLater();
	line.axisY = Q_NULLPTR;
	line.series->deleteLater();
	line.series = Q_NULLPTR;

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
	for( const _line &line : lines )
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
	for( _line &line : lines )
		line.axisY->setRange(min, max);
}

void QBasicChart::setTimeRange(const qreal &minMSec, const qreal &maxMSec)
{
	setTimeRange( QDateTime::fromMSecsSinceEpoch(qint64(minMSec)), QDateTime::fromMSecsSinceEpoch(qint64(maxMSec)) );
}

void QBasicChart::setTimeRange(const QDateTime &minTime, const QDateTime &maxTime)
{
	for( _line &line : lines )
		line.axisX->setRange( minTime, maxTime );
}

void QBasicChart::addValue(const QString &hostname, unsigned long value)
{
	if( not lines.contains(hostname) )
	{
		qDebug() << "En las lineas del gráfico, no existe el hostname " << hostname <<". Quizá se haya borrado.";
		return;
	}

//	qDebug() << "Adding in " << hostname << ": " << value << " at " << QDateTime::currentDateTime();
	lines[hostname].series->append(QDateTime::currentMSecsSinceEpoch(), value);

	for( const QString &host : lines.keys() )
	{
		_line &line = lines[host];

		if( value > line.axisY->max() )
			line.axisY->setMax(value);
		if( !rightLimit.isValid() )
			line.axisX->setMax(QDateTime::currentDateTime());
	}
}

QBasicGraphLineConfigList QBasicChart::basicGraphLineConfigList()
{
	QBasicGraphLineConfigList rtn;
	foreach( const _line &l, lines )
	{
		rtn.append( BasicGraphLineConfig(l.mBasicGraphLineConfig.mRemoteHost, l.mBasicGraphLineConfig.mLineColor) );
	}
	return rtn;
}

void QBasicChart::setInitialTime(const QDateTime &initialTime)
{
	mInitialTime = initialTime;
	for( const QString &host : lines.keys() )
		lines[host].axisX->setMin(initialTime);
}

void QBasicChart::setTimes(const QDateTime &firstTime, const QDateTime &lastTime)
{
	leftLimit = firstTime.isValid() ? firstTime : mInitialTime;
	rightLimit = lastTime;

	for( const QString &host : lines.keys() )
	{
		_line &line = lines[host];
		line.axisX->setRange( leftLimit, rightLimit.isValid() ? rightLimit : QDateTime::currentDateTime() );
	}
}

QBasicChartWidget::QBasicChartWidget(QTabChartHolder *chartHolder)
	: _qChartWidget(mChart = new QBasicChart)
	, mChartHolder(chartHolder)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
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
	emit dobleClic(this);
}

void QBasicChartWidget::heartbeat()
{
	for( BasicGraphLineConfig &cnfg : mGraphicLineConfigList )
	{
		WorkerThread *wt = getFreeThread();
		wt->setHostname(cnfg.mRemoteHost);
		wt->start();
	}
}

void QBasicChartWidget::addHost(const QString &hostname, const QColor &clr)
{
	chart()->addLine(hostname, clr);
	mGraphicLineConfigList.append(BasicGraphLineConfig(hostname, clr));
}

void QBasicChartWidget::delHost(const BasicGraphLineConfig &bglc)
{
	chart()->delLine(bglc);
	mGraphicLineConfigList.removeOne(bglc);
}

void QBasicChartWidget::on_ResultReady(WorkerThread *wt)
{
	addValue( wt->hostname(), wt->resultData().toUInt() );
}
