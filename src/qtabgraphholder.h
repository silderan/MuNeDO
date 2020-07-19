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

#include "graphwidget.h"
#include "projectmanager.h"

#include <QList>
#include <QLabel>

class QTabGraphHolder : public QWidget
{
	QGridLayout *gridLayout_2;
	QVBoxLayout *verticalLayout;
	QScrollArea *scrollArea;
	QWidget *scrollAreaWidgetContents;
	QSlider *horizontalSlider;
	QList<QChartWidget*> mGraphList;
	QToolButton *playButton;
	QLabel *mBackgroundLabel;

	ProjectManager mProjectManager;
	QAction *mAddGraphAction;

	void addBackgroudLabel();
	void removeBackgroudLabel();

	void showContextMenu(const QPoint &pos);

public:
	QTabGraphHolder(QWidget *papi = nullptr);

	QList<QAction*> contextMenuActionList()	{ return QList<QAction*>() << mAddGraphAction;	}


	ProjectManager::ProjectManager_ErrorCode loadProject(const QString &projectFolder);
	void addGraphView();
	QString projectName() const			{ return mProjectManager.projectName();			}
	QString projectFolder() const		{ return mProjectManager.projectFolder();		}
	QString projectDescription() const	{ return mProjectManager.projectDescription();	}

	void addGraphRequest();
	void heartbeat();
};

#endif // QTABGRAPHHOLDER_H