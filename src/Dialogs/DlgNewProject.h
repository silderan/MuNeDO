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

#ifndef DLGNEWTAB_H
#define DLGNEWTAB_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
	class DlgNewProject;
}

class DlgNewTab : public QDialog
{
	Q_OBJECT
	Ui::DlgNewProject *ui;
	QStringList mAllProjectFolders;
	QStringList mOpenedProjectFolders;

	void readAllProjectFolders();
	void refillProjectNameList();

	bool validProjectFolder(const QString &folder) const;
	bool validProjectName(const QString &name) const;
	bool validProjectDesc(const QString &desc) const;
	bool validNewProject() const;

public:
	DlgNewTab(const QStringList &openedProjectFolders, QWidget *parent = nullptr);
	~DlgNewTab();

	QString projectFolder()const;

private slots:
	void on_newName_textChanged(const QString &name);
	void on_newButton_clicked();
	void on_tabNameList_itemDoubleClicked(QListWidgetItem *);
	void on_tabNameList_currentRowChanged(int currentRow);
	void on_openButton_clicked();
	void on_delButton_clicked();

	void on_cancelButton_clicked();
	void on_newFolder_textChanged(const QString &);
	void on_newDescr_textChanged(const QString &);
};

#endif // DLGNEWTAB_H
