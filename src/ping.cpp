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

#include "ping.h"

QString ping(QString dst)
{
	// We declare variables
	HANDLE hIcmpFile;					// Handler
	IPAddr ipaddr = INADDR_NONE;		// Destination address
	char SendData[63] = "1234567890123456789012345678901234567890123456789012345678901";      // The buffer data being sent
	DWORD ReplySize = 0;                    // Buffer Size responses

	// Set the IP-address of the field qlineEdit
	ipaddr = inet_addr(dst.toStdString().c_str());
	hIcmpFile = IcmpCreateFile();   // create a handler

	// Select the buffer memory responses
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

	PongData pongData;
	pongData.originalDest = dst;
	pongData.responces = (PICMP_ECHO_REPLY) malloc(ReplySize);
	pongData.responceCount =  IcmpSendEcho(hIcmpFile, ipaddr, SendData, 63,
										   nullptr, pongData.responces, ReplySize, 1000);

	QString strMessage = pongToString(pongData);

	free(pongData.responces); // frees memory
	return strMessage;
}

unsigned long pingDelay(QString dst)
{
	// We declare variables
	HANDLE hIcmpFile;					// Handler
	IPAddr ipaddr = INADDR_NONE;		// Destination address
	char SendData[63] = "1234567890123456789012345678901234567890123456789012345678901";      // The buffer data being sent
	DWORD ReplySize = 0;                    // Buffer Size responses

	// Set the IP-address of the field qlineEdit
	ipaddr = inet_addr(dst.toStdString().c_str());
	hIcmpFile = IcmpCreateFile();   // create a handler

	// Select the buffer memory responses
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

	PongData pongData;
	pongData.originalDest = dst;
	pongData.responces = (PICMP_ECHO_REPLY) malloc(ReplySize);
	pongData.responceCount =  IcmpSendEcho(hIcmpFile, ipaddr, SendData, 63,
										   nullptr, pongData.responces, ReplySize, 1000);

	unsigned long delay = pongData.responceCount > 0 ? pongData.responces->RoundTripTime : 1000;
	free(pongData.responces); // frees memory
	return delay;
}

QString pongToString(const PongData &pongData)
{
	QString strMessage = "";
	if( pongData.responceCount != 0 )
	{
		// The structure of the echo response
		struct in_addr ReplyAddr;
		ReplyAddr.S_un.S_addr = pongData.responces->Address;

		strMessage += QString("%1 - ").arg(pongData.originalDest);
		strMessage += QString("Received %1 icmp message%2 response%2:\n").arg(pongData.responceCount).arg(pongData.responceCount>1?"s":"");

		strMessage += QString("from %1: %2 ms, status=%3, data %4\n").arg(inet_ntoa(ReplyAddr))
															.arg(pongData.responces->RoundTripTime)
															.arg(pongData.responces->Status)
															.arg((char*)pongData.responces->Data);
	}
	else
	{
		strMessage += "Call to IcmpSendEcho failed.\n";
		strMessage += "IcmpSendEcho returned error: ";
		strMessage += QString::number(GetLastError());
	}
	return strMessage;
}
