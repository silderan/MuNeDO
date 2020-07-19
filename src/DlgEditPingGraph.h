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
#ifndef DLGEDITPINGGRAPH_H
#define DLGEDITPINGGRAPH_H

#include <QDialog>

#include "pingchart.h"

namespace Ui
{
	class DlgEditPingGraph;
}

class QToolButton;

class DlgEditPingGraph : public QDialog
{
	Q_OBJECT

	Ui::DlgEditPingGraph *ui;
	QBasicGraphLineConfigList &mGraphLineConfigList;

	void setButtonColor(QToolButton *btn, const QColor &clr);
	QColor getButtonColor(QToolButton *btn);

public:
	explicit DlgEditPingGraph(QBasicGraphLineConfigList &graphLineConfigList, QWidget *parent);
	~DlgEditPingGraph();

private slots:
	void on_acceptButton_clicked();
};

#endif // DLGEDITPINGGRAPH_H
