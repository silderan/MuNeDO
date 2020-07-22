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
#include "DlgEditPingGraph.h"
#include "ui_DlgEditPingGraph.h"

DlgEditPingGraph::DlgEditPingGraph(QBasicGraphLineConfigList &graphLineConfigList, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgEditPingGraph)
	, mGraphLineConfigList(graphLineConfigList)
{
	ui->setupUi(this);
	setButtonColor(ui->host1Button, mGraphLineConfigList.count() > 0 ? mGraphLineConfigList.at(0).mLineColor : Qt::red);
	setButtonColor(ui->host2Button, mGraphLineConfigList.count() > 1 ? mGraphLineConfigList.at(1).mLineColor : Qt::green);
	setButtonColor(ui->host3Button, mGraphLineConfigList.count() > 2 ? mGraphLineConfigList.at(2).mLineColor : Qt::blue);
	setButtonColor(ui->host4Button, mGraphLineConfigList.count() > 3 ? mGraphLineConfigList.at(3).mLineColor : Qt::darkGray);
	setButtonColor(ui->host5Button, mGraphLineConfigList.count() > 4 ? mGraphLineConfigList.at(4).mLineColor : Qt::cyan);
	setButtonColor(ui->host6Button, mGraphLineConfigList.count() > 5 ? mGraphLineConfigList.at(5).mLineColor : Qt::magenta);
	setButtonColor(ui->host7Button, mGraphLineConfigList.count() > 6 ? mGraphLineConfigList.at(6).mLineColor : Qt::yellow);
	setButtonColor(ui->host8Button, mGraphLineConfigList.count() > 7 ? mGraphLineConfigList.at(7).mLineColor : Qt::darkRed);
	setButtonColor(ui->host9Button, mGraphLineConfigList.count() > 8 ? mGraphLineConfigList.at(8).mLineColor : Qt::darkGreen);
	setButtonColor(ui->host10Button, mGraphLineConfigList.count()> 9 ? mGraphLineConfigList.at(9).mLineColor : Qt::darkBlue);

	if( mGraphLineConfigList.count() > 0 ) ui->host1LineEdit->setText( mGraphLineConfigList.at(0).mRemoteHost );
	if( mGraphLineConfigList.count() > 1 ) ui->host2LineEdit->setText( mGraphLineConfigList.at(1).mRemoteHost );
	if( mGraphLineConfigList.count() > 2 ) ui->host3LineEdit->setText( mGraphLineConfigList.at(2).mRemoteHost );
	if( mGraphLineConfigList.count() > 3 ) ui->host4LineEdit->setText( mGraphLineConfigList.at(3).mRemoteHost );
	if( mGraphLineConfigList.count() > 4 ) ui->host5LineEdit->setText( mGraphLineConfigList.at(4).mRemoteHost );
	if( mGraphLineConfigList.count() > 5 ) ui->host6LineEdit->setText( mGraphLineConfigList.at(5).mRemoteHost );
	if( mGraphLineConfigList.count() > 6 ) ui->host7LineEdit->setText( mGraphLineConfigList.at(6).mRemoteHost );
	if( mGraphLineConfigList.count() > 7 ) ui->host8LineEdit->setText( mGraphLineConfigList.at(7).mRemoteHost );
	if( mGraphLineConfigList.count() > 8 ) ui->host9LineEdit->setText( mGraphLineConfigList.at(8).mRemoteHost );
	if( mGraphLineConfigList.count() > 9 ) ui->host10LineEdit->setText(mGraphLineConfigList.at(9).mRemoteHost );
}


void DlgEditPingGraph::setButtonColor(QToolButton *btn, const QColor &clr)
{
	//Create a solid brush of the desired color
	QBrush brush(clr);
	//Get a copy of the current palette to modify
	QPalette pal = btn->palette();
	//Set all of the color roles but Disabled to our desired color
	pal.setBrush(QPalette::Normal, QPalette::Button, brush);
	pal.setBrush(QPalette::Inactive, QPalette::Button, brush);
	//And finally set the new palette
	btn->setPalette(pal);
}

QColor DlgEditPingGraph::getButtonColor(QToolButton *btn)
{
	return btn->palette().brush(QPalette::Normal, QPalette::Button).color();
}

DlgEditPingGraph::~DlgEditPingGraph()
{
	delete ui;
}

void DlgEditPingGraph::on_acceptButton_clicked()
{
	mGraphLineConfigList.clear();
	if( !ui->host1LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host1LineEdit->text(), getButtonColor(ui->host1Button)) );
	if( !ui->host2LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host2LineEdit->text(), getButtonColor(ui->host2Button)) );
	if( !ui->host3LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host3LineEdit->text(), getButtonColor(ui->host3Button)) );
	if( !ui->host4LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host4LineEdit->text(), getButtonColor(ui->host4Button)) );
	if( !ui->host5LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host5LineEdit->text(), getButtonColor(ui->host5Button)) );
	if( !ui->host6LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host6LineEdit->text(), getButtonColor(ui->host6Button)) );
	if( !ui->host7LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host7LineEdit->text(), getButtonColor(ui->host7Button)) );
	if( !ui->host8LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host8LineEdit->text(), getButtonColor(ui->host8Button)) );
	if( !ui->host9LineEdit->text().isEmpty() ) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host9LineEdit->text(), getButtonColor(ui->host9Button)) );
	if( !ui->host10LineEdit->text().isEmpty()) mGraphLineConfigList.append(BasicGraphLineConfig(ui->host10LineEdit->text(),getButtonColor(ui->host10Button)) );
	accept();
}
