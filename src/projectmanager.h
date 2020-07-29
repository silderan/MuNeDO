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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>
#include "Basic/QIniFile.h"

#define PROJECT_DATA_FNAME "project.data"
#define PROJECT_CHARTS_FNAME "charts.data"

class QChartLineConfigList;
class QBasicChartWidget;
class QBasicChart;
class QChartConfig;
class QChartLineList;

class ProjectManager
{
public:
	enum ProjectManager_ErrorCode
	{
		NoError,
		CannotCreateFolder,
		NoFolderName,
		NoSaved,	// Unknown error at saving time
		NoLoaded,	// Unknown error at loading time
		NoDelete,	// Unknown error at deleting folder time
	};

private:
	QString mProjectName;
	QString mProjectFolder;
	QString mProjectDesc;

	QString mLastErrorArg;
	QString mLastErrorText;
	ProjectManager_ErrorCode mLastErrorCode;

	QString projectPathFolder() const		{ return QString("%1/%2").arg(projectsRootFolder()).arg(projectFolder());	}
	QString projectDataFileName() const		{ return QString("%1/%2").arg(projectPathFolder()).arg(PROJECT_DATA_FNAME);	}
	QString projectChartsFolder() const		{ return projectPathFolder();	}
	QString projectChartFolder(const QString &chartID) const		{ return QString("%1/%2").arg(projectPathFolder()).arg(chartID);	}
	QString projectChartsFileName(const QString &chartID) const	{ return QString("%1/%2/%3").arg(projectPathFolder()).arg(chartID).arg(PROJECT_CHARTS_FNAME);	}
	QString projectSeriesFileName(const QString &chartID, const QString &lineID) const	{ return QString("%1/%2/%3.series").arg(projectPathFolder()).arg(chartID).arg(lineID); }

public:
	ProjectManager(const QString &name, const QString &folder, const QString &description = QString());
	ProjectManager(const QString &folder);

	static QString projectsRootFolder()	{ return "projects";	}
	QString projectName()const			{ return mProjectName;	}
	QString projectFolder()const		{ return mProjectFolder;}
	QString projectDescription()const	{ return mProjectDesc;	}

	static QStringList allProjectFolders();
	ProjectManager_ErrorCode createNewProject();

	const QString &lastErrorText() const	{ return mLastErrorText;	}

	ProjectManager_ErrorCode loadProject(const QString &folder);
	ProjectManager_ErrorCode loadProject();
	ProjectManager_ErrorCode saveProject() const;
	QList<QChartConfig> loadCharts();
	void saveCharts(const QList<QChartConfig> &chartConfigList);
	bool saveChartConfig(const QChartConfig &chartConfig) const;
	bool loadChartConfig(const QString &chartID, QChartConfig &chartConfig) const;
	bool saveLineSeries(const QString &chartID, const QString &lineID, const QByteArray &series) const;
	QByteArray loadLineSeries(const QString &chartID, const QString &lineID) const;
	ProjectManager_ErrorCode deleteProject();
};

#endif // PROJECTMANAGER_H
