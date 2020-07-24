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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QTabChartHolder.h"
#include "Dialogs/DlgNewTab.h"
#include "projectmanager.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, heartTimer(this)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect( &heartTimer, &QTimer::timeout, this, &MainWindow::heartbeat );
	heartTimer.start(1000);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::heartbeat()
{
	for( int i = 0; i < ui->tabWidget->count(); ++i )
		static_cast<QTabChartHolder*>(ui->tabWidget->widget(i))->heartbeat();
}

// Action del menú para añadir una nueva pestaña de proyecto.
// Abre el diálogo para escoger un proyecto existente o crear uno nuevo.
void MainWindow::on_addProjectAction_triggered()
{
	QStringList openedFolders;
	DlgNewTab dlg(openedProjectFolders(), this);
	if( dlg.exec() )
	{
		QTabChartHolder *tab = new QTabChartHolder(Q_NULLPTR);
		if( tab->loadProject(dlg.projectFolder()) == ProjectManager::ProjectManager_ErrorCode::NoError )
			ui->tabWidget->addTab(tab, tab->projectName());
		else
			delete tab;
	}
}

void MainWindow::on_addChartAction_triggered()
{
	if( QTabChartHolder *tab = static_cast<QTabChartHolder*>(ui->tabWidget->currentWidget()) )
		tab->editChart(tab->addPingChart(false));
	else
		qDebug() << "currentTabWidget() is not a QTabChartHolder instance";
}
QWidget *MainWindow::currentViewport()
{
	return Q_NULLPTR;
}

QStringList MainWindow::openedProjectFolders() const
{
	QStringList rtnLst;
	for( int i = 0; i < ui->tabWidget->count(); ++i )
		rtnLst.append( static_cast<const QTabChartHolder*>(ui->tabWidget->widget(i))->projectFolder() );
	return rtnLst;
}

void MainWindow::on_tabWidget_tabBarDoubleClicked(int index)
{
	ui->tabWidget->removeTab(index);
}

//void MainWindow::on_contextMenuRequested(const QPoint &chartWidgetPoint)
//{
//	if( QBasicChartWidget *chartWidget = static_cast<QBasicChartWidget*>(sender()) )
//	{
//		QMenu contextMenu( tr("Chart context menu"), this);
//		QAction editChart( tr("Editar gráfico"), this);
//		connect( &editChart, &QAction::triggered, this, [&]() {this->edit(chartWidget);} );
//		contextMenu.addAction( &editChart );

//		contextMenu.exec(chartWidget->mapToGlobal(chartWidgetPoint));
//	}
//	else
//		qDebug() << "sender() is not a QBasicChartWidget instance";
//}

//void MainWindow::showEditDialog(QBasicChartWidget *chartWidget)
//{
//	chartWidget->editChart();
//}
