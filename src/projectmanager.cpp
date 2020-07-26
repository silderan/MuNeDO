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

#include "projectmanager.h"

#include <QObject>
#include <QDir>

#include "Basic/QIniFile.h"
#include "QBasicChartWidget.h"

ProjectManager::ProjectManager(const QString &folder)
	: mProjectName(folder)
	, mProjectFolder(folder)
	, mProjectDesc("")
	, mLastErrorCode(ProjectManager_ErrorCode::NoError)
{

}

ProjectManager::ProjectManager(const QString &name, const QString &folder, const QString &description)
	: mProjectName(name)
	, mProjectFolder(folder)
	, mProjectDesc(description.isEmpty() ? QObject::tr("Proyecto para %1").arg(mProjectName) : description)
	, mLastErrorCode(ProjectManager_ErrorCode::NoError)
{

}

QStringList ProjectManager::allProjectFolders()
{
	return QDir(projectsRootFolder()).entryList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot);
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::createNewProject()
{
	QDir dir;
	QString newFolder = QString("%1/%2").arg(projectsRootFolder()).arg(mProjectFolder);
	if( !dir.mkpath(newFolder) )
	{
		mLastErrorCode = ProjectManager_ErrorCode::CannotCreateFolder;
		mLastErrorText = QObject::tr("No se ha podido crear el directorio %1").arg(newFolder);
		mLastErrorArg = mProjectFolder;
		return ProjectManager_ErrorCode::NoError;
	}
	return saveProject();
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::loadProject(const QString &folder)
{
	mProjectFolder = folder;
	return loadProject();
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::loadProject()
{
	QIniData data;

	if( !QIniFile::load(projectDataFileName(), &data) )
		return ProjectManager_ErrorCode::NoLoaded;

	mProjectName = data["project_name"];
	mProjectDesc = data["project_desc"];

	return ProjectManager_ErrorCode::NoError;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::saveProject() const
{
	Q_ASSERT( !mProjectFolder.isEmpty() );
	Q_ASSERT( !mProjectDesc.isEmpty() );
	Q_ASSERT( !mProjectName.isEmpty() );

	QIniData data;
	data["project_name"] = mProjectName;
	data["project_desc"] = mProjectDesc;

	if( !QIniFile::save(projectDataFileName(), data) )
		return ProjectManager_ErrorCode::NoSaved;

	return ProjectManager_ErrorCode::NoError;
}

bool ProjectManager::saveChartConfig(int chartID, const QChartConfig &chartConfig) const
{
	QDir dir;
	QString newFolder = projectChartsFolder(chartID);
	dir.mkpath(newFolder);

	QIniData chartData;
	chartConfig.save(chartData);

	return QIniFile::save(projectChartsFileName(chartID), chartData);
}

bool ProjectManager::loadChartConfig(int chartID, QChartConfig &chartConfig) const
{
	QIniData chartData;
	if( QIniFile::load(projectChartsFileName(chartID), &chartData) )
		return chartConfig.load(chartData);

	return false;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::deleteProject()
{
	QDir dir(projectPathFolder());
	if( !dir.removeRecursively() )
		return ProjectManager::ProjectManager_ErrorCode::NoDelete;
	return ProjectManager::ProjectManager_ErrorCode::NoError;
}
