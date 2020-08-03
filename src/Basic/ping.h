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
#include <QVariant>

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <QThread>
#include <QVariant>

struct PongData
{
	QString originalDest;
	DWORD responceCount;
	PICMP_ECHO_REPLY responces;
};

QString pongToString(const PongData &pongData);
QString	ping(QString dst);
unsigned long pingDelay(QString dst);

void addAsyncPingDelay(const QString &pingID, const QString &dst, std::function<void(const QString &, const QVariant &, const QDateTime &)> fnc, bool paused);
void removeAsyncPing(const QString &pingID);

void setPaused(const QString &id, bool paused);
void executeAllThreads();
void setThreadPaused(const QString &id, bool paused);

#endif // PING_H
