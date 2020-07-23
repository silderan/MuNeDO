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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QBasicChartWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	QTimer heartTimer;

private slots:
	void heartbeat();
	void on_addTabAction_triggered();
	void on_delTabAction_triggered();
	void on_addGraphAction_triggered();
	void on_delGraphAction_triggered();
	void on_tabWidget_tabBarDoubleClicked(int index);
	void on_contextMenuRequested(const QPoint &chartWidgetPoint);
	void showEditDialog(QBasicChartWidget *chartWidget);

private:
	Ui::MainWindow *ui;
	QWidget *currentViewport();
	QStringList openedProjectFolders()const;
};
#endif // MAINWINDOW_H
