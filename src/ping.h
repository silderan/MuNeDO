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

#ifndef PING_H
#define PING_H

#include <QString>

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

struct PongData
{
	QString originalDest;
	DWORD responceCount;
	PICMP_ECHO_REPLY responces;
};

QString pongToString(const PongData &pongData);
QString	ping(QString dst);
unsigned long pingDelay(QString dst);

#endif // PING_H
