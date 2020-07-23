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

QTabChartHolder::QTabChartHolder(QWidget *papi)
	: QWidget(papi)
	, gridLayout_2(Q_NULLPTR)
	, verticalLayout(Q_NULLPTR)
	, scrollArea(Q_NULLPTR)
	, scrollAreaWidgetContents(Q_NULLPTR)
	, horizontalSlider(Q_NULLPTR)
	, playButton(Q_NULLPTR)
	, mProjectManager("")
	, mAddGraphAction( new QAction( tr("Añadir Gráfica"), this) )
	, mPlaying(false)
{
}

ProjectManager::ProjectManager_ErrorCode QTabChartHolder::loadProject(const QString &projectFolder)
{
	ProjectManager::ProjectManager_ErrorCode err = mProjectManager.loadProject(projectFolder);

	if( err == ProjectManager::ProjectManager_ErrorCode::NoError )
	{
		// ToDo: Remove all graph widgets.

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
	}
	return err;
}

void QTabChartHolder::addChart(QBasicChartWidget *chartWidget)
{
	chartWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	chartWidget->setMinimumHeight(200);
	verticalLayout->addWidget(chartWidget);
	mChartList.append(chartWidget);

	chartWidget->setInitialTime(mInitialTime);
	chartWidget->setTimes(mLeftTime, mRightTime);
}

void QTabChartHolder::play()
{
	if( !mPlaying )
	{
		mPlaying = true;
		if( !mInitialTime.isValid() )
		{
			mLeftTime = mInitialTime = QDateTime::currentDateTime();
			mRightTime = mInitialTime.addSecs(1);
		}
		for( QBasicChartWidget *graph : mChartList )
			graph->setInitialTime(mInitialTime);
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
		for( QBasicChartWidget *graph : mChartList )
			graph->heartbeat();
}