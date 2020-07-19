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

#include "qtabgraphholder.h"

#include <QMenu>

QTabGraphHolder::QTabGraphHolder(QWidget *papi)
	: QWidget(papi)
	, gridLayout_2(Q_NULLPTR)
	, verticalLayout(Q_NULLPTR)
	, scrollArea(Q_NULLPTR)
	, scrollAreaWidgetContents(Q_NULLPTR)
	, horizontalSlider(Q_NULLPTR)
	, playButton(Q_NULLPTR)
	, mBackgroundLabel(Q_NULLPTR)
	, mProjectManager("")
	, mAddGraphAction( new QAction( tr("Añadir Gráfica"), this) )
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect( this, &QTabGraphHolder::customContextMenuRequested, this, &QTabGraphHolder::showContextMenu);
	connect( mAddGraphAction, &QAction::triggered, this, &QTabGraphHolder::addGraphRequest );
}

void QTabGraphHolder::addBackgroudLabel()
{
	Q_ASSERT(mGraphList.isEmpty());
	mBackgroundLabel = new QLabel( tr("\tClick derecho para abrir menú y añadir gráficos") );
	verticalLayout->addWidget(mBackgroundLabel);
}

void QTabGraphHolder::removeBackgroudLabel()
{
	mBackgroundLabel->deleteLater();
	mBackgroundLabel = Q_NULLPTR;
}

void QTabGraphHolder::showContextMenu(const QPoint &pos)
{
	QMenu contextMenu( tr("Basic context menu"), this);

	for( QAction *action : contextMenuActionList() )
		contextMenu.addAction(action);

	contextMenu.exec(mapToGlobal(pos));
}

ProjectManager::ProjectManager_ErrorCode QTabGraphHolder::loadProject(const QString &projectFolder)
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

		gridLayout_2->addWidget(horizontalSlider, 1, 0, 1, 1);

		playButton = new QToolButton(this);
		playButton->setObjectName(QString::fromUtf8("playButton"));

		gridLayout_2->addWidget(playButton, 1, 1, 1, 1);
		addBackgroudLabel();
	}
	return err;
}

// TODO: Esta función debe cambiar mucho porque debe decidir/recibir el tipo de gráfico a crear.
void QTabGraphHolder::addGraphView()
{
	removeBackgroudLabel();
	QPingChartWidget *newGraph = new QPingChartWidget(this);
	newGraph->setObjectName(QString::fromUtf8("Graph"));

	newGraph->setMinimumHeight(200);
	verticalLayout->addWidget(newGraph);
	mGraphList.append(newGraph);

	newGraph->editGraph();
}

// Called when a new graph wants to be added in holder.
// Usually, through context menu.
void QTabGraphHolder::addGraphRequest()
{
	addGraphView();
}

void QTabGraphHolder::heartbeat()
{
	for( QChartWidget *graph : mGraphList )
		graph->heartbeat();
}
