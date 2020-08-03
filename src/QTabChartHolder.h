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

#ifndef QTABCHARTHOLDER_H
#define QTABCHARTHOLDER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QtWidgets/QSlider>
#include <QToolButton>

#include "QPingChartWidget.h"
#include "ProjectManager.h"
#include "QMultipleHandleSlider.h"

#include <QList>
#include <QLabel>

class QTabChartHolder : public QWidget
{
Q_OBJECT

	QGridLayout *gridLayoutBase;
	QVBoxLayout *verticalLayout;
	QScrollArea *scrollArea;
	QWidget *scrollAreaWidgetContents;
	QSlider *iniTimeSlider;
	QList<QBasicChartWidget*> mChartList;
	QToolButton *playButton;

	ProjectManager mProjectManager;
	QAction *mAddChartAction;
	bool mPlaying;
	QDateTime mInitialTime;
	QDateTime mLeftTime;
	QDateTime mRightTime;
	QString mIniTimeID;
	QString mEndTimeID;
	QMultipleHandleSlider *mTimeSlider;

	QBasicChartWidget *addChart(const QChartConfig &chartConfig, QBasicChartWidget *chartWidget);

	void onTimeSliderValueChanged(int value, const QString &id);
	void onChartEndTimeChanged(const QDateTime &endTime);

public:
	QTabChartHolder(QWidget *papi = nullptr);

	ProjectManager::ProjectManager_ErrorCode loadProject(const QString &projectFolder);
	QString projectName() const			{ return mProjectManager.projectName();			}
	QString projectFolder() const		{ return mProjectManager.projectFolder();		}
	QString projectDescription() const	{ return mProjectManager.projectDescription();	}

	void saveCharts() const;
	void loadCharts();

	void saveSeries() const;
	void loadSeries();
	QPingChartWidget *addPingChart(const QChartConfig &chartConfig, bool save = true);
	void editChart(QBasicChartWidget *chartWidget);
	void removeChart(QBasicChartWidget *chartWidget);
	void togglePlaying();
	void closeProject();

signals:
	void timeSliderValueChanged(const QDateTime &iniTime, const QDateTime &endTime);
};

#endif // QTABCHARTHOLDER_H
