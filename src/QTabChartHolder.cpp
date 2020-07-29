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

#include <QMenu>
#include <QDebug>

#include "Dialogs/DlgEditPingChart.h"

QTabChartHolder::QTabChartHolder(QWidget *papi)
	: QWidget(papi)
	, gridLayout_2(Q_NULLPTR)
	, verticalLayout(Q_NULLPTR)
	, scrollArea(Q_NULLPTR)
	, scrollAreaWidgetContents(Q_NULLPTR)
	, horizontalSlider(Q_NULLPTR)
	, playButton(Q_NULLPTR)
	, mProjectManager("")
	, mAddChartAction( new QAction( tr("Añadir Gráfica"), this) )
	, mPlaying(false)
{
}

ProjectManager::ProjectManager_ErrorCode QTabChartHolder::loadProject(const QString &projectFolder)
{
	ProjectManager::ProjectManager_ErrorCode err = mProjectManager.loadProject(projectFolder);

	if( err == ProjectManager::ProjectManager_ErrorCode::NoError )
	{
		// ToDo: Remove all widgets.

		gridLayout_2 = new QGridLayout(this);
		gridLayout_2->setSpacing(0);
		gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
		gridLayout_2->setContentsMargins(0, 0, 0, 0);
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
		gridLayout_2->addWidget(scrollArea, 0, 0, 1, 2);

		horizontalSlider = new QSlider(this);
		horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
		horizontalSlider->setOrientation(Qt::Horizontal);
		horizontalSlider->setRange(0, 0);
		connect( horizontalSlider, &QSlider::valueChanged, this, &QTabChartHolder::leftLimitChanged );

		gridLayout_2->addWidget(horizontalSlider, 1, 0, 1, 1);

		playButton = new QToolButton(this);
		playButton->setObjectName(QString::fromUtf8("playButton"));
		connect( playButton, &QToolButton::clicked, this, &QTabChartHolder::play );

		gridLayout_2->addWidget(playButton, 1, 1, 1, 1);

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
					chartWidget->delHost(oldLine);

			for( const QLineConfig &host : newChrtCnfg.mLines  )
				chartWidget->addHost(host, false);
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
				chart->addHost(line, true)
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
	return chartWidget;
}

QPingChartWidget *QTabChartHolder::addPingChart(const QChartConfig &chartConfig, bool save)
{
	QPingChartWidget *chartWidget = new QPingChartWidget(this);
	chartWidget->setObjectName(QString::fromUtf8("PingChart"));
	addChart(chartConfig, static_cast<QBasicChartWidget*>(chartWidget));

	for( const QLineConfig &lineConfig : chartConfig.mLines )
		chartWidget->addHost(lineConfig, false);

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

void QTabChartHolder::play()
{
	if( !mPlaying )
	{
		mPlaying = true;
		if( !mInitialTime.isValid() )
			mLeftTime = mInitialTime = QDateTime::currentDateTime();

		for( QBasicChartWidget *chart : mChartList )
			chart->setInitialTime(mInitialTime);
	}
	else
		mPlaying = false;
}
void QTabChartHolder::leftLimitChanged(int newVal)
{

}

void QTabChartHolder::heartbeat()
{
	if( mPlaying )
		for( QBasicChartWidget *chart : mChartList )
			chart->heartbeat();
}
