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

#include "QTabChartHolder.h"
#include "QTabChartHolder.h"

#include <QMenu>
#include <QDebug>

#include "Dialogs/DlgEditPingChart.h"
#include "Basic/ping.h"

QTabChartHolder::QTabChartHolder(QWidget *papi)
	: QWidget(papi)
	, gridLayoutBase(Q_NULLPTR)
	, verticalLayout(Q_NULLPTR)
	, scrollArea(Q_NULLPTR)
	, scrollAreaWidgetContents(Q_NULLPTR)
	, iniTimeSlider(Q_NULLPTR)
	, playButton(Q_NULLPTR)
	, mProjectManager("")
	, mAddChartAction( new QAction( tr("Añadir Gráfica"), this) )
	, mPlaying(false)
	, mIniTimeID("iniTimeID")
	, mEndTimeID("endTimeID")
{
}

ProjectManager::ProjectManager_ErrorCode QTabChartHolder::loadProject(const QString &projectFolder)
{
	ProjectManager::ProjectManager_ErrorCode err = mProjectManager.loadProject(projectFolder);

	if( err == ProjectManager::ProjectManager_ErrorCode::NoError )
	{
		gridLayoutBase = new QGridLayout(this);
		gridLayoutBase->setSpacing(0);
		gridLayoutBase->setObjectName(QString::fromUtf8("gridLayout_2"));
		gridLayoutBase->setContentsMargins(0, 0, 0, 0);
		scrollArea = new QScrollArea(this);
		scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
		scrollArea->setWidgetResizable(true);
		scrollAreaWidgetContents = new QWidget();
		scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
		scrollAreaWidgetContents->setGeometry(QRect(0, 0, 946, 548));
		verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		verticalLayout->setContentsMargins(0, 2, 0, 2);
		scrollArea->setWidget(scrollAreaWidgetContents);
		gridLayoutBase->addWidget(scrollArea, 0, 0, 1, 2);

		mTimeSlider = new QMultipleHandleSlider(this);
		mTimeSlider->setObjectName(QString::fromUtf8("timeSlider"));
		mTimeSlider->setRange(QDateTime::currentSecsSinceEpoch()-1, QDateTime::currentSecsSinceEpoch()+1, false);

		QColor clr(Qt::blue);

		mTimeSlider->addHandle( mIniTimeID, clr, clr.lighter(), QSize(14, 25) );
		mTimeSlider->setValue( QDateTime::currentSecsSinceEpoch()-1, mIniTimeID );

		mTimeSlider->addHandle( mEndTimeID, clr, clr.lighter(), QSize(14, 25) );
		mTimeSlider->setValue( QDateTime::currentSecsSinceEpoch()+1, mEndTimeID );

		mTimeSlider->addMiddleHandle( mIniTimeID, mEndTimeID, clr.darker(), clr );

		connect( mTimeSlider, &QMultipleHandleSlider::valueChanged, this, &QTabChartHolder::onTimeSliderValueChanged );

		gridLayoutBase->addWidget(mTimeSlider, 1, 0, 1, 1);

		playButton = new QToolButton(this);
		playButton->setObjectName(QString::fromUtf8("playButton"));
		connect( playButton, &QToolButton::clicked, this, &QTabChartHolder::togglePlaying );

		playButton->setIcon(QIcon(":/images/play.png"));
		gridLayoutBase->addWidget(playButton, 1, 1, 1, 1);

		loadCharts();
//		loadSeries();
	}
	return err;
}

void QTabChartHolder::editChart(QBasicChartWidget *chartWidget)
{
	QChartConfig newChrtCnfg = chartWidget->getChartConfig();

	DlgEditPingChart dlg(newChrtCnfg , this);
	if( dlg.exec() )
	{
		if( dlg.removeChart() )
			removeChart(chartWidget);
		else
		{
			chartWidget->setChartID(newChrtCnfg.mChartID);
			chartWidget->setChartName(newChrtCnfg.mChartName);

			for( const QChartLine &oldLine : chartWidget->chartLines() )
				if( !newChrtCnfg.containsLine(oldLine.mID) )
					chartWidget->delChartLine(oldLine);

			for( const QLineConfig &host : newChrtCnfg.mLines  )
				if( !host.mID.isEmpty() && !host.mRemoteHost.isEmpty() && !host.mLabel.isEmpty() )
					chartWidget->addChartLine(host, false, !mPlaying);
		}
		saveCharts();
	}
}

void QTabChartHolder::saveCharts() const
{
	for( const QBasicChartWidget *chart : mChartList )
	{
		QChartConfig chartConfig = chart->getChartConfig();
		mProjectManager.saveChartConfig(chartConfig);
	}
}

void QTabChartHolder::loadCharts()
{
	for( const QChartConfig &chartConfig : mProjectManager.loadCharts() )
	{
		if( chartConfig.mChartType == "ping" )
			addPingChart(chartConfig, false);
		else
			qDebug() << "Chart type " << chartConfig.mChartType << " unknown. Maybe it's for a future version";
	}
}

void QTabChartHolder::saveSeries() const
{
	for( const QBasicChartWidget *chart : mChartList )
	{
		for( const QChartLine &line : chart->chartLines() )
		{
			mProjectManager.saveLineSeries(chart->chartID(), line.mID, line.saveSeries());
		}
	}
}

void QTabChartHolder::loadSeries()
{
	for( QBasicChartWidget *chart : mChartList )
	{
		long long maxX = 0;
		long long maxY = 0;
		long long minX = 0;
		for( QChartLine &line : chart->chartLines() )
		{
			if( !line.mIsOld )
			{
				chart->addChartLine(line, true, true)
					.loadSeries( mProjectManager.loadLineSeries(chart->chartID(), line.mID), maxY, maxX, minX );
			}
		}
		if( maxX == minX )
			maxX+= 1000;
		chart->setInitialTime( mInitialTime = QDateTime::fromMSecsSinceEpoch(minX) );
		chart->setMaxY(maxY);
	}
}

QBasicChartWidget *QTabChartHolder::addChart(const QChartConfig &chartConfig, QBasicChartWidget *chartWidget)
{
	chartWidget->setMinimumHeight(200);
	verticalLayout->addWidget(chartWidget);
	mChartList.append(chartWidget);
	chartWidget->setChartID(chartConfig.mChartID);
	chartWidget->setChartName(chartConfig.mChartName);

	chartWidget->setInitialTime(mInitialTime);
	chartWidget->setTimeRange(mLeftTime, mRightTime);
	connect( chartWidget, &QBasicChartWidget::dobleClic, this, &QTabChartHolder::editChart );
	connect( chartWidget, &QBasicChartWidget::endTimeUpdated, this, &QTabChartHolder::onChartEndTimeChanged );
	return chartWidget;
}

void QTabChartHolder::onTimeSliderValueChanged(int value, const QString &id)
{
	Q_UNUSED(value);
	Q_UNUSED(id);

	QDateTime minTime = mLeftTime = QDateTime::fromSecsSinceEpoch(mTimeSlider->value(mIniTimeID));
	QDateTime maxTime = mRightTime = QDateTime::fromSecsSinceEpoch(mTimeSlider->value(mEndTimeID));

	emit timeSliderValueChanged(mLeftTime, mRightTime);
	if( minTime.toSecsSinceEpoch() == qint64(mTimeSlider->minimum()) )
		minTime = QDateTime();

	if( maxTime.toSecsSinceEpoch() == qint64(mTimeSlider->maximum()) )
		maxTime = QDateTime();

	for( QBasicChartWidget *chartWidget : mChartList )
	{
		chartWidget->setTimeRange(minTime, maxTime);
		chartWidget->updateChartMaxAxis();
	}
}

void QTabChartHolder::onChartEndTimeChanged(const QDateTime &endTime)
{
	mTimeSlider->setMaximum(endTime.toSecsSinceEpoch(), true);
}

QPingChartWidget *QTabChartHolder::addPingChart(const QChartConfig &chartConfig, bool save)
{
	QPingChartWidget *chartWidget = new QPingChartWidget(this);
	chartWidget->setObjectName(QString::fromUtf8("PingChart"));
	addChart(chartConfig, static_cast<QBasicChartWidget*>(chartWidget));

	for( const QLineConfig &lineConfig : chartConfig.mLines )
		chartWidget->addChartLine(lineConfig, false, !mPlaying);

	if( save )
		saveCharts();
	return chartWidget;
}

void QTabChartHolder::removeChart(QBasicChartWidget *chartWidget)
{
	mChartList.removeOne(chartWidget);

	chartWidget->deleteLater();
	mProjectManager.deleteChart(chartWidget->chartID());

	saveCharts();
}

void QTabChartHolder::togglePlaying()
{
	if( !mPlaying )
	{
		mPlaying = true;
		if( !mInitialTime.isValid() )
			mLeftTime = mInitialTime = QDateTime::currentDateTime();

		for( QBasicChartWidget *chart : mChartList )
		{
			chart->setInitialTime(mInitialTime);
			chart->setPaused(false);
		}
		playButton->setIcon(QIcon(":/images/stop.png"));
	}
	else
	{
		playButton->setIcon(QIcon(":/images/play.png"));
		mPlaying = false;
		for( QBasicChartWidget *chart : mChartList )
			chart->setPaused(true);
	}
}

void QTabChartHolder::closeProject()
{
	if( mPlaying )
		togglePlaying();

	for( QBasicChartWidget *chart : mChartList )
		chart->deleteLater();
	mChartList.clear();
}
