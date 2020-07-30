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

QChartLine &QPingChartWidget::addChartLine(const QLineConfig &lineConfig, bool isOld, bool paused)
{
	QChartLine &line = QBasicChartWidget::addChartLine(lineConfig, isOld, paused);
	addAsyncPingDelay(lineConfig.mID, lineConfig.mRemoteHost, [this](const QString &id, const QVariant &val) {this->chart()->onResult(id, val);}, paused );
	return line;
}

void QPingChartWidget::delChartLine(const QLineConfig &lineConfig)
{
	removeAsyncPing(lineConfig.mID);
	QBasicChartWidget::delChartLine(lineConfig);
}
