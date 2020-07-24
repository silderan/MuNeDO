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
	return saveProject(QList<QBasicChartLineConfigList>());
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

ProjectManager::ProjectManager_ErrorCode ProjectManager::saveProject(const QList<QBasicChartLineConfigList>  &chartLineList) const
{
	Q_ASSERT( !mProjectFolder.isEmpty() );
	Q_ASSERT( !mProjectDesc.isEmpty() );
	Q_ASSERT( !mProjectName.isEmpty() );

	QIniData data;
	data["project_name"] = mProjectName;
	data["project_desc"] = mProjectDesc;

	if( !QIniFile::save(projectDataFileName(), data) )
		return ProjectManager_ErrorCode::NoSaved;

	for( int chartID = chartLineList.count()-1; chartID >= 0; --chartID )
	{
		ProjectManager_ErrorCode err = saveProjectChart(chartID, chartLineList.at(chartID));
		if( err != ProjectManager_ErrorCode::NoError )
			return err;
	}
	return ProjectManager_ErrorCode::NoError;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::deleteProject()
{
	QDir dir(projectPathFolder());
	if( !dir.removeRecursively() )
		return ProjectManager::ProjectManager_ErrorCode::NoDelete;
	return ProjectManager::ProjectManager_ErrorCode::NoError;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::saveProjectChart(int chartID, const QBasicChartLineConfigList &chartLines) const
{
	Q_ASSERT( !mProjectFolder.isEmpty() );

	QDir dir;
	QString newFolder = projectChartsFolder(chartID);
	dir.mkpath(newFolder);

	int i = 0;
	QIniData data;
	for( const BasicChartLineConfig &lineConfig : chartLines )
	{
		data[QString("line_%1_name").arg(i)] = lineConfig.mRemoteHost;
		data[QString("line_%1_color").arg(i)] = QString("%1:%2:%3").arg(lineConfig.mLineColor.red()).arg(lineConfig.mLineColor.green()).arg(lineConfig.mLineColor.blue());
		++i;
	}
	if( !QIniFile::save(projectChartsFileName(chartID), data) )
		return ProjectManager_ErrorCode::NoSaved;
	return ProjectManager_ErrorCode::NoError;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::loadProjectChart(int chartID, QBasicChartLineConfigList &chartLines) const
{
	QIniData data;

	if( !QIniFile::load(projectChartsFileName(chartID), &data) )
		return ProjectManager_ErrorCode::NoLoaded;

	for( int i = 0; data.contains(QString("line_%1_name").arg(i)); ++i )
	{
		QStringList colors = data[QString("line_%1_color").arg(i)].split(":");
		QString name = data[QString("line_%1_name").arg(i)];
		if( colors.count() == 3 )
			chartLines.append(BasicChartLineConfig(name, QColor(colors[0].toInt(),	// Red
																colors[1].toInt(),	// Green
																colors[2].toInt()) ));// Blue
		else
			return ProjectManager_ErrorCode::NoLoaded;
	}
	return ProjectManager_ErrorCode::NoError;
}

ProjectManager::ProjectManager_ErrorCode ProjectManager::loadProjectCharts(QList<QBasicChartLineConfigList> &chartLineList) const
{
	for( int chartID = 0; ; ++chartID )
	{
		QBasicChartLineConfigList chartLines;
		ProjectManager_ErrorCode err = loadProjectChart(chartID, chartLines);
		if( err != ProjectManager_ErrorCode::NoError )
			return err;
		chartLineList.append(chartLines);
	}
}
