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

#include "QPingChartWidget.h"
#include "Basic/ping.h"

#include <QMenu>

#include <QDebug>

#include "Dialogs/DlgEditPingGraph.h"


void QPingChartWidget::editChart()
{
	QBasicGraphLineConfigList newList = basicGraphLineConfigList();
	DlgEditPingGraph dlg(newList , this);
	if( dlg.exec() )
	{
		QBasicGraphLineConfigList oldList = basicGraphLineConfigList();
		for( BasicGraphLineConfig &oldHost : oldList )
			if( !newList.contains(oldHost.mRemoteHost) )
				delHost(oldHost);

		for( BasicGraphLineConfig &host : newList  )
			addHost(host);
	}
}

void QPingChartWidget::on_DoJob(WorkerThread *wt)
{
	wt->setResultData( QVariant().fromValue(pingDelay(wt->hostname())) );
}
