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

#include "qtabgraphholder.h"
#include "dlgnewtab.h"
#include "projectmanager.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, heartTimer(this)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect( &heartTimer, SIGNAL(timeout()), this, SLOT(heartbeat()) );
	heartTimer.start(1000);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::heartbeat()
{
	for( int i = 0; i < ui->tabWidget->count(); ++i )
		static_cast<QTabGraphHolder*>(ui->tabWidget->widget(i))->heartbeat();
}

// Action del menú para añadir una nueva pestaña de proyecto.
// Abre el diálogo para escoger un proyecto existente o crear uno nuevo.
void MainWindow::on_addTabAction_triggered()
{
	QStringList openedFolders;
	DlgNewTab dlg(openedProjectFolders(), this);
	if( dlg.exec() )
	{
		QTabGraphHolder *tab = new QTabGraphHolder(Q_NULLPTR);
		if( tab->loadProject(dlg.projectFolder()) == ProjectManager::ProjectManager_ErrorCode::NoError )
			ui->tabWidget->addTab(tab, tab->projectName());
		else
			delete tab;
	}
}

void MainWindow::on_delTabAction_triggered()
{

}

void MainWindow::on_addGraphAction_triggered()
{
	if( QTabGraphHolder*tab = static_cast<QTabGraphHolder*>(ui->tabWidget->currentWidget()) )
	{
		tab->addGraphView();
	}
}

void MainWindow::on_delGraphAction_triggered()
{

}

QWidget *MainWindow::currentViewport()
{
	return Q_NULLPTR;
}

QStringList MainWindow::openedProjectFolders() const
{
	QStringList rtnLst;
	for( int i = 0; i < ui->tabWidget->count(); ++i )
		rtnLst.append( static_cast<const QTabGraphHolder*>(ui->tabWidget->widget(i))->projectFolder() );
	return rtnLst;
}

void MainWindow::on_tabWidget_tabBarDoubleClicked(int index)
{
	ui->tabWidget->removeTab(index);
}
