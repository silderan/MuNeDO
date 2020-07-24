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
#include "DlgEditPingChart.h"
#include "ui_DlgEditPingChart.h"

#include <QColorDialog>

DlgEditPingChart::DlgEditPingChart(QBasicChartLineConfigList &chartLineConfigList, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgEditPingChart)
	, mChartLineConfigList(chartLineConfigList)
	, mRemoveChart(false)
{
	ui->setupUi(this);
	setButtonColor(ui->host1Button, mChartLineConfigList.count() > 0 ? mChartLineConfigList.at(0).mLineColor : Qt::red);
	setButtonColor(ui->host2Button, mChartLineConfigList.count() > 1 ? mChartLineConfigList.at(1).mLineColor : Qt::green);
	setButtonColor(ui->host3Button, mChartLineConfigList.count() > 2 ? mChartLineConfigList.at(2).mLineColor : Qt::blue);
	setButtonColor(ui->host4Button, mChartLineConfigList.count() > 3 ? mChartLineConfigList.at(3).mLineColor : Qt::darkGray);
	setButtonColor(ui->host5Button, mChartLineConfigList.count() > 4 ? mChartLineConfigList.at(4).mLineColor : Qt::cyan);
	setButtonColor(ui->host6Button, mChartLineConfigList.count() > 5 ? mChartLineConfigList.at(5).mLineColor : Qt::magenta);
	setButtonColor(ui->host7Button, mChartLineConfigList.count() > 6 ? mChartLineConfigList.at(6).mLineColor : Qt::yellow);
	setButtonColor(ui->host8Button, mChartLineConfigList.count() > 7 ? mChartLineConfigList.at(7).mLineColor : Qt::darkRed);
	setButtonColor(ui->host9Button, mChartLineConfigList.count() > 8 ? mChartLineConfigList.at(8).mLineColor : Qt::darkGreen);
	setButtonColor(ui->host10Button, mChartLineConfigList.count()> 9 ? mChartLineConfigList.at(9).mLineColor : Qt::darkBlue);

	if( mChartLineConfigList.count() > 0 ) ui->host1LineEdit->setText( mChartLineConfigList.at(0).mRemoteHost );
	if( mChartLineConfigList.count() > 1 ) ui->host2LineEdit->setText( mChartLineConfigList.at(1).mRemoteHost );
	if( mChartLineConfigList.count() > 2 ) ui->host3LineEdit->setText( mChartLineConfigList.at(2).mRemoteHost );
	if( mChartLineConfigList.count() > 3 ) ui->host4LineEdit->setText( mChartLineConfigList.at(3).mRemoteHost );
	if( mChartLineConfigList.count() > 4 ) ui->host5LineEdit->setText( mChartLineConfigList.at(4).mRemoteHost );
	if( mChartLineConfigList.count() > 5 ) ui->host6LineEdit->setText( mChartLineConfigList.at(5).mRemoteHost );
	if( mChartLineConfigList.count() > 6 ) ui->host7LineEdit->setText( mChartLineConfigList.at(6).mRemoteHost );
	if( mChartLineConfigList.count() > 7 ) ui->host8LineEdit->setText( mChartLineConfigList.at(7).mRemoteHost );
	if( mChartLineConfigList.count() > 8 ) ui->host9LineEdit->setText( mChartLineConfigList.at(8).mRemoteHost );
	if( mChartLineConfigList.count() > 9 ) ui->host10LineEdit->setText(mChartLineConfigList.at(9).mRemoteHost );

	connect( ui->host1Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host2Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host3Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host4Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host5Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host6Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host7Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host8Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host9Button, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( ui->host10Button,&QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
}


void DlgEditPingChart::setButtonColor(QToolButton *button, const QColor &clr)
{
	QPalette pal = button->palette();
	pal.setColor(QPalette::Button, clr);
	button->setAutoFillBackground(true);
	button->setPalette(pal);
	button->setProperty("Color", clr);
}

QColor DlgEditPingChart::getButtonColor(QToolButton *btn)
{
	return btn->property("Color").value<QColor>();
}

void DlgEditPingChart::chooseColor()
{
	QToolButton *btn = static_cast<QToolButton*>(sender());
	setButtonColor(btn, QColorDialog::getColor(getButtonColor(btn), this, tr("Color de la linea")));
}

DlgEditPingChart::~DlgEditPingChart()
{
	delete ui;
}

void DlgEditPingChart::on_acceptButton_clicked()
{
	mChartLineConfigList.clear();
	if( !ui->host1LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host1LineEdit->text(), getButtonColor(ui->host1Button)) );
	if( !ui->host2LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host2LineEdit->text(), getButtonColor(ui->host2Button)) );
	if( !ui->host3LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host3LineEdit->text(), getButtonColor(ui->host3Button)) );
	if( !ui->host4LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host4LineEdit->text(), getButtonColor(ui->host4Button)) );
	if( !ui->host5LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host5LineEdit->text(), getButtonColor(ui->host5Button)) );
	if( !ui->host6LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host6LineEdit->text(), getButtonColor(ui->host6Button)) );
	if( !ui->host7LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host7LineEdit->text(), getButtonColor(ui->host7Button)) );
	if( !ui->host8LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host8LineEdit->text(), getButtonColor(ui->host8Button)) );
	if( !ui->host9LineEdit->text().isEmpty() ) mChartLineConfigList.append(BasicChartLineConfig(ui->host9LineEdit->text(), getButtonColor(ui->host9Button)) );
	if( !ui->host10LineEdit->text().isEmpty()) mChartLineConfigList.append(BasicChartLineConfig(ui->host10LineEdit->text(),getButtonColor(ui->host10Button)) );
	accept();
}

void DlgEditPingChart::on_removeChartButton_clicked()
{
	mRemoveChart = true;
	accept();
}
