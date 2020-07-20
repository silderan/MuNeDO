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

#include "pingchart.h"
#include "ping.h"
#include "qtabgraphholder.h"

#include <QMenu>

#include <QDebug>

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

	if( !leftLimit.isValid() )
	{
		leftLimit = QDateTime::currentDateTime();
		rightLimit = QDateTime::currentDateTime().addMSecs(1);
	}
	line.axisX->setRange( leftLimit, rightLimit );

	if( lines.count() != 1 )
	{
		line.axisY->hide();
		line.axisX->hide();
	}

	return line;
}

void QBasicChart::addValue(const QString &hostname, unsigned long value)
{
	Q_ASSERT( lines.contains(hostname) );

	lines[hostname].series->append(QDateTime::currentMSecsSinceEpoch(), value);

	for( const QString &host : lines.keys() )
	{
		_line &line = lines[host];

		if( value > line.axisY->max() )
			line.axisY->setMax(value);
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

void QChartWidget::showContextMenu(const QPoint &pos)
{
	QMenu contextMenu( tr("Graph context menu"), this);

	for( QAction *action : mGraphHolder->contextMenuActionList() )
		contextMenu.addAction(action);

	QAction editGraph( tr("Editar gráfico"), this);
	connect( &editGraph, &QAction::triggered, this, &QChartWidget::editGraph );
	contextMenu.addAction( &editGraph );

	contextMenu.exec(mapToGlobal(pos));
}

WorkerThread *QChartWidget::getFreeThread()
{
	for( WorkerThread *hilo : mAllThreads )
		if( hilo->isFinished() )
			return hilo;

	WorkerThread *workerThread = new WorkerThread();
	mAllThreads.append(workerThread);
	connect(workerThread, &WorkerThread::doJob, this, &QChartWidget::on_DoJob);
	connect(workerThread, &WorkerThread::resultReady, this, &QChartWidget::on_ResultReady);

	return workerThread;
}

void QChartWidget::heartbeat()
{
	for( BasicGraphLineConfig &cnfg : mGraphicLineConfigList )
	{
		WorkerThread *wt = getFreeThread();
		wt->setHostname(cnfg.mRemoteHost);
		wt->start();
	}
}

void QChartWidget::addHost(const QString &hostname, const QColor &clr)
{
	chart()->addLine(hostname, clr);
	mGraphicLineConfigList.append(BasicGraphLineConfig(hostname, clr));
}

#include "DlgEditPingGraph.h"
void QPingChartWidget::editGraph()
{
	DlgEditPingGraph dlg(mGraphicLineConfigList, this);
	if( dlg.exec() )
	{
		for( BasicGraphLineConfig &bglc : mGraphicLineConfigList )
			chart()->addLine(bglc);
	}
}

void QPingChartWidget::on_DoJob(WorkerThread *wt)
{
	wt->setResultData( QVariant().fromValue(pingDelay(wt->hostname())) );
}

void QPingChartWidget::on_ResultReady(WorkerThread *wt)
{
	chart()->addValue( wt->hostname(), wt->resultData().toUInt() );
}
