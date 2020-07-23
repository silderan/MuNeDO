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

#ifndef QTABGRAPHHOLDER_H
#define QTABGRAPHHOLDER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QtWidgets/QSlider>
#include <QToolButton>

#include "QPingChartWidget.h"
#include "projectmanager.h"

#include <QList>
#include <QLabel>

class QTabChartHolder : public QWidget
{
Q_OBJECT

	QGridLayout *gridLayout_2;
	QVBoxLayout *verticalLayout;
	QScrollArea *scrollArea;
	QWidget *scrollAreaWidgetContents;
	QSlider *horizontalSlider;
	QList<QBasicChartWidget*> mChartList;
	QToolButton *playButton;

	ProjectManager mProjectManager;
	QAction *mAddGraphAction;
	bool mPlaying;
	QDateTime mInitialTime;
	QDateTime mLeftTime;
	QDateTime mRightTime;

	void leftLimitChanged(int newVal);

public:
	QTabChartHolder(QWidget *papi = nullptr);

	ProjectManager::ProjectManager_ErrorCode loadProject(const QString &projectFolder);
	QString projectName() const			{ return mProjectManager.projectName();			}
	QString projectFolder() const		{ return mProjectManager.projectFolder();		}
	QString projectDescription() const	{ return mProjectManager.projectDescription();	}


	void addChart(QBasicChartWidget *chartWidget);
	void play();
	void heartbeat();
};

#endif // QTABGRAPHHOLDER_H