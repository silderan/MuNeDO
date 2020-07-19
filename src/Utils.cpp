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

#include "Utils.h"
#include <QTableWidget>

QDate Utils::fromStringDate(const char *date)
{
	// 15/12/2018
	return QDate( 1000*(date[6]-'0') + 100*(date[7]-'0') + 10*(date[8]-'0') + (date[9]-'0'),
				  10*(date[3]-'0') + (date[4]-'0'),
				  10*(date[0]-'0') + (date[1]-'0') );
}

QDate Utils::fromStringDate(const QString &date)
{
	if( date.count() >= 10 )
		return fromStringDate(date.toLatin1().constData());
	return QDate();
}

QTime Utils::fromStringTime(const char *date)
{
	// 11:20:30
	return QTime( 10*(date[0]-'0') + (date[1]-'0'),
				  10*(date[3]-'0') + (date[4]-'0'),
				  10*(date[6]-'0') + (date[7]-'0') );
}

QTime Utils::fromStringTime(const QString &time)
{
	if( time.count() >= 8 )
		return fromStringTime(time.toLatin1().constData());
	return QTime();
}

QString Utils::toStringDate(const QDate &date)
{
	return date.toString("dd/MM/yyyy");
}

QStringList Utils::allColumnTexts(const QAbstractItemModel *model, int col, Qt::ItemDataRole role)
{
	QStringList rtn;
	if( col < model->columnCount() )
	{
		rtn.reserve(model->rowCount());
		QString s;
		for( int row = model->rowCount(); row >= 0; --row )
		{
			s = model->index(row, col).data(role).toString();
			if( !s.isEmpty() && !rtn.contains(s) )
				rtn.append(s);
		}
	}
	return rtn;
}

#include <QMessageBox>
void Utils::raiseWarning(QWidget *papi, const QString &info, QString title)
{
	QMessageBox::warning(papi, title.isEmpty() ? papi->windowTitle() : title, info );
}

void Utils::raiseInfo(QWidget *papi, const QString &info, QString title)
{
	QMessageBox::information( papi, title.isEmpty() ? papi->windowTitle() : title, info );
}


void Utils::moveTableRow(QTableWidget *table, int oldRow, int newRow)
{
	if( oldRow == newRow )
		return;
	if( (oldRow < 0) || (oldRow >= table->rowCount()) )
		return;
	if( (newRow < 0) || (newRow >= table->rowCount()) )
		return;
	if( newRow < oldRow )
		oldRow++;
	else
		newRow++;
	table->insertRow(newRow);
	for( int col = table->columnCount()-1; col >= 0; col-- )
		table->setItem(newRow, col, table->takeItem(oldRow, col));

	table->removeRow(oldRow);
}

int Utils::selectedRow(const QTableWidget *table)
{
	QList<QTableWidgetItem*> items = table->selectedItems();
	return items.isEmpty() ? -1 : items.first()->row();
}
