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
#include "Basic/Utils.h"

DlgEditPingChart::DlgEditPingChart(QChartConfig &chartConfig, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgEditPingChart)
	, mChartConfig(chartConfig)
	, mRemoveChart(false)
{
	ui->setupUi(this);

	mControlLines.append( _controlLine(ui->host1Button, ui->id1LineEdit, ui->host1LineEdit, ui->name1LineEdit) );
	mControlLines.append( _controlLine(ui->host2Button, ui->id2LineEdit, ui->host2LineEdit, ui->name2LineEdit) );
	mControlLines.append( _controlLine(ui->host3Button, ui->id3LineEdit, ui->host3LineEdit, ui->name3LineEdit) );
	mControlLines.append( _controlLine(ui->host4Button, ui->id4LineEdit, ui->host4LineEdit, ui->name4LineEdit) );
	mControlLines.append( _controlLine(ui->host5Button, ui->id5LineEdit, ui->host5LineEdit, ui->name5LineEdit) );
	mControlLines.append( _controlLine(ui->host6Button, ui->id6LineEdit, ui->host6LineEdit, ui->name6LineEdit) );
	mControlLines.append( _controlLine(ui->host7Button, ui->id7LineEdit, ui->host7LineEdit, ui->name7LineEdit) );
	mControlLines.append( _controlLine(ui->host8Button, ui->id8LineEdit, ui->host8LineEdit, ui->name8LineEdit) );
	mControlLines.append( _controlLine(ui->host9Button, ui->id9LineEdit, ui->host9LineEdit, ui->name9LineEdit) );
	mControlLines.append( _controlLine(ui->host10Button,ui->id10LineEdit,ui->host10LineEdit,ui->name10LineEdit) );

	setupLine( mControlLines[0], mChartConfig.mLines.count() > 0 ? &mChartConfig.mLines.at(0) : Q_NULLPTR, Qt::red );
	setupLine( mControlLines[1], mChartConfig.mLines.count() > 1 ? &mChartConfig.mLines.at(1) : Q_NULLPTR, Qt::green );
	setupLine( mControlLines[2], mChartConfig.mLines.count() > 2 ? &mChartConfig.mLines.at(2) : Q_NULLPTR, Qt::blue );
	setupLine( mControlLines[3], mChartConfig.mLines.count() > 3 ? &mChartConfig.mLines.at(3) : Q_NULLPTR, Qt::darkGray );
	setupLine( mControlLines[4], mChartConfig.mLines.count() > 4 ? &mChartConfig.mLines.at(4) : Q_NULLPTR, Qt::cyan );
	setupLine( mControlLines[5], mChartConfig.mLines.count() > 5 ? &mChartConfig.mLines.at(5) : Q_NULLPTR, Qt::magenta );
	setupLine( mControlLines[6], mChartConfig.mLines.count() > 6 ? &mChartConfig.mLines.at(6) : Q_NULLPTR, Qt::yellow );
	setupLine( mControlLines[7], mChartConfig.mLines.count() > 7 ? &mChartConfig.mLines.at(7) : Q_NULLPTR, Qt::darkRed );
	setupLine( mControlLines[8], mChartConfig.mLines.count() > 8 ? &mChartConfig.mLines.at(8) : Q_NULLPTR, Qt::darkGreen );
	setupLine( mControlLines[9], mChartConfig.mLines.count() > 9 ? &mChartConfig.mLines.at(9) : Q_NULLPTR, Qt::darkBlue );

	connect( ui->chartName, &QLineEdit::textChanged, [=](const QString &txt) { if( ui->chartID->isEnabled() ) ui->chartID->setText(QString("%1").arg(Utils::safeText(txt))); } );

	if( !mChartConfig.mChartID.isEmpty() )
	{
		ui->chartID->setText(mChartConfig.mChartID);
		ui->chartID->setEnabled(false);
	}
	ui->chartName->setText(mChartConfig.mChartName);
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

void DlgEditPingChart::setupLine(DlgEditPingChart::_controlLine &controlLine, const QLineConfig *lineConfig, const QColor &defaultClr)
{
	if( lineConfig )
	{
		setButtonColor(controlLine.clr, lineConfig ? lineConfig->mLineColor : defaultClr);

		controlLine.id->setText( lineConfig->mID );
		controlLine.label->setText( lineConfig->mLabel );
		controlLine.host->setText( lineConfig->mRemoteHost );
	}
	else
		setButtonColor(controlLine.clr, defaultClr);

	connect( controlLine.clr, &QToolButton::clicked, this, &DlgEditPingChart::chooseColor );
	connect( controlLine.label, &QLineEdit::textChanged, [=](const QString &txt) { controlLine.id->setText(QString("id_%1").arg(Utils::safeText(txt)));} );
}

void DlgEditPingChart::getLine(const DlgEditPingChart::_controlLine &controlLine)
{
	if( !controlLine.label->text().isEmpty() && !controlLine.host->text().isEmpty() )
		mChartConfig.mLines.append(
			QLineConfig( controlLine.id->text(),
						 controlLine.label->text(),
						 controlLine.host->text(),
						 "ping",
						 "",
						 QStringList(),
						 getButtonColor(controlLine.clr)) );
}

DlgEditPingChart::~DlgEditPingChart()
{
	delete ui;
}

void DlgEditPingChart::on_acceptButton_clicked()
{
	mChartConfig.mChartID = ui->chartID->text();
	mChartConfig.mChartName = ui->chartName->text();
	mChartConfig.mChartType = "ping";
	mChartConfig.mLines.clear();
	for( const DlgEditPingChart::_controlLine &ctrLine : mControlLines )
		getLine(ctrLine);
	accept();
}

void DlgEditPingChart::on_removeChartButton_clicked()
{
	mRemoveChart = true;
	accept();
}
