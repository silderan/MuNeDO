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
#include "QBasicChartWidget.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

#include "QTabChartHolder.h"
#include "Basic/ping.h"

//void WorkerThread::run()
//{
//	QDateTime cur = QDateTime::currentDateTime().addSecs(1);
//	while( QDateTime::currentDateTime() < cur )
//		;
//	setResultData( QVariant().fromValue(1000) );
//	emit resultReady(this);
//}

QT_CHARTS_USE_NAMESPACE

bool QLineConfig::load(const QString &preKey, const QIniData &iniData)
{
	if( iniData.contains(QString("%1id").arg(preKey)) )
	{
		QStringList colors = iniData[QString("%1color").arg(preKey)].split(":");
		if( colors.count() == 3 )
		{
			mID					= iniData[QString("%1id").arg(preKey)];
			mLabel				= iniData[QString("%1label").arg(preKey)];
			mRemoteHost			= iniData[QString("%1remote").arg(preKey)];
			mRequestType		= iniData[QString("%1type").arg(preKey)];
			mRequestValue		= iniData[QString("%1value").arg(preKey)];
			mRequestParameters	= iniData[QString("%1params").arg(preKey)].split(",");
			mLineColor			= QColor(colors[0].toInt(),		// Red
										 colors[1].toInt(),		// Green
										 colors[2].toInt());	// Blue
			return true;
		}
	}
	return false;
}

void QLineConfig::save(const QString &preKey, QIniData &iniData) const
{
	iniData[QString("%1id").arg(preKey)]		= mID;
	iniData[QString("%1label").arg(preKey)]		= mLabel;
	iniData[QString("%1remote").arg(preKey)]	= mRemoteHost;
	iniData[QString("%1type").arg(preKey)]		= mRequestType;
	iniData[QString("%1value").arg(preKey)]		= mRequestValue;
	iniData[QString("%1params").arg(preKey)]	= mRequestParameters.join(",");
	iniData[QString("%1color").arg(preKey)]		= QString("%1:%2:%3").arg(mLineColor.red())
																	 .arg(mLineColor.green())
																	 .arg(mLineColor.blue());
}

bool QChartConfig::load(const QIniData &data)
{
	mChartType	= data["chartType"];
	mChartID	= data["chartID"];
	mChartName	= data["chartName"];

	QLineConfig lineConfig;
	for( int lineID = 0 ; lineConfig.load(QString("line_%1_").arg(lineID), data); ++lineID )
		mLines.append(lineConfig);
	return true;
}

QIniData &QChartConfig::save(QIniData &data) const
{
	Q_ASSERT( !mChartID.isEmpty() );
	Q_ASSERT( !mChartType.isEmpty() );

	data["chartType"]	= mChartType;
	data["chartID"]		= mChartID;
	data["chartName"]	= mChartName;

	int lineID = 0;
	for( const QLineConfig &lineConfig : mLines )
		lineConfig.save(QString("line_%1_").arg(lineID++), data);

	return data;
}

bool QChartConfig::containsLine(const QString &lineID) const
{
	for( const QLineConfig &lineConfig : mLines )
		if( lineConfig.mID == lineID )
			return true;
	return false;
}

QBasicChart::QBasicChart(const QString &chartType, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
	, mChartType(chartType)
{
	setMargins( QMargins() );
}

QChartConfig QBasicChart::getChartConfig() const
{
	QChartConfig chartConfig;

	chartConfig.mChartType	= mChartType;
	chartConfig.mChartID	= mChartID;
	chartConfig.mChartName	= mChartName;

	for( const QChartLine &line : lines )
		chartConfig.mLines.append(line);

	return chartConfig;
}

QChartLine &QBasicChart::addLine(const QLineConfig &lineConfig, bool isOld, bool paused)
{
	if( !isOld )
	{
		QChartLine &line = lines[lineConfig.mID];
		if( line.isValid() && !lineConfig.mIsOld )
		{
			line.changeColor(lineConfig.mLineColor);
			line.setLabel(lineConfig.mLabel);
			return line;
		}
	}
	lines.append(QChartLine(lineConfig));
	QChartLine &line = lines.last();
	if( (line.mIsOld = isOld) )
	{
		line.mID += "_old";
	}

	line = lineConfig;
	line.series = new _qLineSeries();
	line.axisX = new _qTimeAxis();
	line.axisY = new _qValueAxis();

	line.axisX->setTickCount(1);
	line.axisX->setFormat("dd/MM/yy hh:mm:ss");
	line.axisY->setLabelFormat("%ims");

	addSeries(line.series);

	addAxis(line.axisX, Qt::AlignBottom);
	addAxis(line.axisY, Qt::AlignLeft);

	line.series->attachAxis(line.axisX);
	line.series->attachAxis(line.axisY);

	line.setLabel(line.mLabel);
	line.changeColor(line.mLineColor);

	line.axisX->setRange( leftLimit.isValid() ? leftLimit : mInitialTime.isValid() ? mInitialTime : QDateTime::currentDateTime(),
						  rightLimit.isValid() ? rightLimit : QDateTime::currentDateTime().addMSecs(1) );

	// If it's the first line, it must be visible. The other ones must be hiden.
	if( lines.count() != 1 )
	{
		line.axisY->hide();
		line.axisX->hide();
	}
	return line;
}

void QBasicChart::delLine(const QLineConfig &lineConfig)
{
	QChartLine &line = lines[lineConfig.mID];
	removeAxis( line.axisX );
	removeAxis( line.axisY );
	removeSeries( line.series );

	line.axisX->deleteLater();	line.axisX = Q_NULLPTR;
	line.axisY->deleteLater();	line.axisY = Q_NULLPTR;
	line.series->deleteLater();	line.series = Q_NULLPTR;

	lines.remove( lineConfig.mID );
	removeAsyncPing( lineConfig.mID );

	// If line is visible, as it's gona be removed, visibility goes to another one.
	if( lines.count() && !lines.first().axisX->isVisible() )
	{
		lines.first().axisX->show();
		lines.first().axisY->show();
	}
	updateChartMaxAxis();
}

// Called when time axis is modified or when a line is removed.
// This updates Y axis values acordigly to the visible values.
void QBasicChart::updateChartMaxAxis()
{
	qreal newMax = 0;
	qreal minTime = leftLimit.isValid() ? leftLimit.toMSecsSinceEpoch() : 0;
	qreal maxTime = rightLimit.isValid() ? rightLimit.toMSecsSinceEpoch() : QDateTime::currentMSecsSinceEpoch();
	qreal curTime;
	for( const QChartLine &line : lines )
	{
		for( int i = line.series->count()-1; i>=0; --i )
		{
			curTime = line.series->at(i).x();
			if( (curTime >= minTime) && (curTime <= maxTime) )
			{
				if( newMax < line.series->at(i).y() )
					newMax = line.series->at(i).y();
			}
		}
	}
	setValueRange(0, newMax);
}

void QBasicChart::setValueRange(const qreal &min, const qreal &max)
{
	for( QChartLine &line : lines )
		line.axisY->setRange(min, max);
}

void QBasicChart::setTimeRange(const qreal &minMSec, const qreal &maxMSec)
{
	setTimeRange( QDateTime::fromMSecsSinceEpoch(qint64(minMSec)), QDateTime::fromMSecsSinceEpoch(qint64(maxMSec)) );
}

void QBasicChart::setTimeRange(const QDateTime &minTime, const QDateTime &maxTime)
{
	for( QChartLine &line : lines )
		line.axisX->setRange( minTime, maxTime );
}

void QBasicChart::addValue(const QString &lineID, unsigned long value)
{
	if( !lines.contains(lineID) )
	{
		qDebug() << "Ninguna linea con ID " << lineID <<". Quizá se haya borrado.";
		return;
	}

//	qDebug() << "Adding in " << hostname << ": " << value << " at " << QDateTime::currentDateTime();
	lines[lineID].series->append(QDateTime::currentMSecsSinceEpoch(), value);

	for( QChartLine &line : lines )
	{
		if( value > line.axisY->max() )
			line.axisY->setMax(value);
		if( !rightLimit.isValid() )
			line.axisX->setMax(QDateTime::currentDateTime());
	}
}

void QBasicChart::setInitialTime(const QDateTime &initialTime)
{
	if( mInitialTime != initialTime )
	{
		mInitialTime = initialTime;
		Q_ASSERT(mInitialTime.isValid());
		for( QChartLine &line : lines )
			line.axisX->setMin(initialTime);
	}
}

void QBasicChart::setPaused(bool paused)
{
	for( const QChartLine &line : lines )
		setThreadPaused(line.mID, paused);
}

void QBasicChart::setTimes(const QDateTime &firstTime, const QDateTime &lastTime)
{
	leftLimit = firstTime.isValid() ? firstTime : mInitialTime;
	rightLimit = lastTime;

	for( QChartLine &line : lines )
		line.axisX->setRange( leftLimit, rightLimit.isValid() ? rightLimit : QDateTime::currentDateTime() );
}

void QBasicChart::setMaxY(long long maxY)
{
	for( QChartLine &line : lines )
		line.axisY->setMax(maxY);
}

QBasicChartWidget::QBasicChartWidget(QTabChartHolder *chartHolder, const QString &chartType)
	: _qChartWidget(mChart = new QBasicChart(chartType))
	, mChartHolder(chartHolder)
{
}

void QBasicChartWidget::deleteLater()
{
	for( const QChartLine &line : chartLines() )
		delHost(line);
	_qChartWidget::deleteLater();
}

void QBasicChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit dobleClic(this);
}

char toHexChar(char i)
{
	const char *converter = "0123456789ABCDEF";
	return converter[i&0xF];
}

void toHexChar(long long value, QByteArray &out)
{
	int ini = out.count();
	while( value )
	{
		out.append( toHexChar(static_cast<char>(value)) );
		value >>= 4;
	}
	// Reverse chars.
	for( int fin = out.count()-1; ini < fin; ini++, fin-- )
	{
		char tmp = out[ini];
		out[ini] = out[fin];
		out[fin] = tmp;
	}
}

char fromHex(char c)
{
	return (c <= '9') ? (c - '0') : (c - 'A' + 10);
}

// To save space, series are stored in a bytearraylist
// Every first 16 elements are the time stamp of the first 'x' value
// The nexts 4 bytes are the array length
// The rest of elements in the list are 1000ms away from first time-stamp.
// This will be a char * to be more eficient to save and load from disc.
QByteArray QChartLine::saveSeries() const
{
	QByteArray data;
	long long initialTime = 0;
	int seriesLength = 0;
	long long time;
	long long value;
	for( int i = 0; i < series->count(); ++i )
	{
		time = static_cast<long long>(series->at(i).x())/1000;
		value = static_cast<long long>(series->at(i).y());

		if( !data.count() || initialTime+seriesLength != time )
		{
			initialTime = time;
			if( data.count() )
				data.append('\n');
			toHexChar(time, data);
			data.append( ':' );
			seriesLength = 0;
		}

		// Value:
		toHexChar(value, data);
		data.append( ' ' );
		seriesLength++;
	}
	data.append('\n');
	return data;
}

void QChartLine::loadSeries(const QByteArray &data, long long &maxY, long long &maxX, long long &minX)
{
	long long initialTime = 0;
	long long value;

	for( int i = 0; i < data.count(); ++i )
	{
		if( data.at(i) == '\n' )
		{
			initialTime = 0;
			i++;
		}
		if( initialTime == 0 )
		{
			for( initialTime = 0; (data.count() > i) && (data[i] != ':'); i++ )
				initialTime = (initialTime << 4) + fromHex(data[i]);

			initialTime *= 1000;
			i++;
			if( !initialTime )
				continue;
		}

		// Value.
		for( value = 0; (data.count() > i) && (data[i] != ' '); i++ )
			value = (value << 4) + fromHex(data[i]);

		series->append(initialTime, value);

		if( minX == 0 )
			minX = initialTime;

		if( maxX < initialTime )
			maxX = initialTime;

		if( maxY < value )
			maxY = value;

		initialTime+=1000;
	}
}
