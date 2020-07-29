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
#ifndef DLGEDITPINGCHART_H
#define DLGEDITPINGCHART_H

#include <QDialog>

#include <QLineEdit>
#include <QToolButton>

#include "QPingChartWidget.h"

namespace Ui
{
	class DlgEditPingChart;
}

class QToolButton;

class DlgEditPingChart : public QDialog
{
Q_OBJECT

	Ui::DlgEditPingChart *ui;
	QChartLineConfigList &mChartLineConfigList;

	bool mRemoveChart;

	void setButtonColor(QToolButton *btn, const QColor &clr);
	QColor getButtonColor(QToolButton *btn);
	void chooseColor();

	struct _controlLine
	{
		QToolButton *clr;
		QLineEdit *id;
		QLineEdit *host;
		QLineEdit *label;
		_controlLine(QToolButton *clr, QLineEdit *id, QLineEdit *host, QLineEdit *label)
			: clr(clr)
			, id(id)
			, host(host)
			, label(label)
		{	}
		_controlLine(const _controlLine &other)
			: clr(other.clr)
			, id(other.id)
			, host(other.host)
			, label(other.label)
		{	}
	};
	QList<_controlLine> mControlLines;

	void setupLine(_controlLine &controlLine, const QLineConfig *lineConfig, const QColor &defaultClr);
	void getLine(const _controlLine &controlLine);
public:
	explicit DlgEditPingChart(QChartLineConfigList &chartLineConfigList, QWidget *parent);
	~DlgEditPingChart();
	bool removeChart() const	{ return mRemoveChart;	}

private slots:
	void on_acceptButton_clicked();
	void on_removeChartButton_clicked();
};

#endif // DLGEDITPINGCHART_H
