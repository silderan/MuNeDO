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

#include "DlgNewTab.h"
#include "ui_dlgnewtab.h"

#include <QDir>
#include <QMessageBox>

#include "projectmanager.h"

DlgNewTab::DlgNewTab(const QStringList &openedProjectFolders, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgNewTab)
	, mOpenedProjectFolders(openedProjectFolders)
{
	ui->setupUi(this);
	ui->newButton->setDisabled(true);
	ui->openButton->setDisabled(true);
	readAllProjectFolders();
	refillProjectNameList();
}

DlgNewTab::~DlgNewTab()
{
	delete ui;
}

QString DlgNewTab::projectFolder() const
{
	return ui->newFolder->text();
}

void DlgNewTab::readAllProjectFolders()
{
	mAllProjectFolders = ProjectManager::allProjectFolders();
}

void DlgNewTab::refillProjectNameList()
{
	ui->tabNameList->clear();

	ProjectManager pm("");
	for( const QString &folder : mAllProjectFolders )
	{
		if( pm.loadProject(folder) == ProjectManager::ProjectManager_ErrorCode::NoError )
		{
			QString caption = QString("%1, %2").arg(pm.projectName()).arg(pm.projectDescription());
			if( mOpenedProjectFolders.contains(pm.projectFolder()) )
				caption += " (Ya abierto)";
			QListWidgetItem *item = new QListWidgetItem(caption);
			item->setData(Qt::UserRole+0, pm.projectName());
			item->setData(Qt::UserRole+1, pm.projectFolder());
			item->setData(Qt::UserRole+2, pm.projectDescription());
			ui->tabNameList->addItem(item);
		}
	}

}

bool DlgNewTab::validProjectFolder(const QString &folder) const
{
	// Check for valid name is quite complex because it's very OS dependant.
	// So, let's give up and check if it's created fine on newButton_clicked.
	return !folder.isEmpty();
}

bool DlgNewTab::validProjectName(const QString &name) const
{
	return !name.isEmpty();
}
bool DlgNewTab::validProjectDesc(const QString &desc) const
{
	return !desc.isEmpty();
}

bool DlgNewTab::validNewProject() const
{
	QString name = ui->newName->text();
	QString folder = ui->newFolder->text();
	QString desc = ui->newDescr->text();

	// newButton is only enabled if name is valid and this is not a current project.
	return  validProjectName(name) &&
			validProjectDesc(desc) &&
			validProjectFolder(folder) && !mAllProjectFolders.contains(folder, Qt::CaseInsensitive);
}

void DlgNewTab::on_newName_textChanged(const QString &name)
{
	ui->newFolder->setText( QString(name).replace(QRegExp("[^a-zA-Z0-9_]"), "_") );
	ui->newDescr->setText( name.isEmpty() ? "" : tr("Proyecto para %1").arg(name) );
}
void DlgNewTab::on_newFolder_textChanged(const QString &)
{
	ui->newButton->setEnabled( validNewProject() );
}
void DlgNewTab::on_newDescr_textChanged(const QString &)
{
	ui->newButton->setEnabled( validNewProject() );
}

void DlgNewTab::on_newButton_clicked()
{
	ProjectManager pm(ui->newName->text(), ui->newFolder->text(), ui->newDescr->text());
	if( pm.createNewProject() == ProjectManager::ProjectManager_ErrorCode::NoError )
	{
		accept();
	}
	else
		QMessageBox::warning(this, tr("Creando nuevo proyecto"), pm.lastErrorText(), QMessageBox::Ok);
}

void DlgNewTab::on_tabNameList_itemDoubleClicked(QListWidgetItem *)
{
	if( !mOpenedProjectFolders.contains(ui->newFolder->text()) )
		accept();
}

void DlgNewTab::on_tabNameList_currentRowChanged(int currentRow)
{
	QListWidgetItem *item = ui->tabNameList->item(currentRow);
	ui->newName->setText(item->data(Qt::UserRole+0).toString());
	ui->newFolder->setText(item->data(Qt::UserRole+1).toString());
	ui->newDescr->setText(item->data(Qt::UserRole+2).toString());
	if( mOpenedProjectFolders.contains(ui->newFolder->text()) )
	{
		ui->openButton->setEnabled( false );
		ui->delButton->setEnabled( false );
	}
	else
	{
		ui->openButton->setEnabled( true );
		ui->delButton->setEnabled( true );
	}
}

void DlgNewTab::on_openButton_clicked()
{
	on_tabNameList_itemDoubleClicked(ui->tabNameList->currentItem());
}

void DlgNewTab::on_delButton_clicked()
{
	if( QListWidgetItem *item = ui->tabNameList->currentItem() )
	{
		QString projectName = ui->tabNameList->currentItem()->data(Qt::UserRole+0).toString();
		QString projectFolder = ui->tabNameList->currentItem()->data(Qt::UserRole+1).toString();
		QString projectDesc = ui->tabNameList->currentItem()->data(Qt::UserRole+2).toString();

		if( QMessageBox::question(this, tr("Borrando proyecto"), tr("¿Seguro que quieres borrar el proyecto %1").arg(ui->newName->text())) == QMessageBox::Yes )
		{
			ProjectManager pm(projectName, projectFolder, projectDesc);
			if( pm.deleteProject() == ProjectManager::ProjectManager_ErrorCode::NoError )
				delete item;
		}
	}
}

void DlgNewTab::on_cancelButton_clicked()
{
	close();
}
