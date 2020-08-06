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
#include "QMultipleHandleSlider.h"

#include <QMouseEvent>
#include <QPixmap>
#include <QCursor>
#include <QProxyStyle>
#include <QPainter>
#include <QPainterPath>

#include <QDebug>

class SliderProxy : public QProxyStyle
{
public:
	virtual int pixelMetric( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const override
	{
		switch( metric )
		{
		case PM_SliderThickness  : return 25;
		case PM_SliderLength     : return 25;
		default                  : return (QProxyStyle::pixelMetric(metric,option,widget));
		}
	}
};

int QMultipleHandleSliderHandle::minHandleXPos(bool limitedByOthers) const
{
	return parentSlider()->minHandleXPos(*this, limitedByOthers);
}

int QMultipleHandleSliderHandle::maxHandleXPos(bool limitedByOthers) const
{
	return parentSlider()->maxHandleXPos(*this, limitedByOthers);
}

int QMultipleHandleSliderHandle::handleYPos()
{
	// That is for center handle in the slider.
	return (parentSlider()->height() - height()) / 2;
}

int QMultipleHandleSliderHandle::checkValidValue(int value)
{
	return parentSlider()->checkValidValue(this, value);
}

void QMultipleHandleSliderHandle::handleFollowMouse()
{
	moveHandeToPos(parentWidget()->mapFromGlobal(QCursor::pos()).x());
}

// Move hande to position relative to its parent.
// This functions fits perfecly when handle must follow the mouse.
// pos parameter is the position of the mouse relative to slider (not global possition)
// This funcion also recalculates value and emits valueChange if necessary.
void QMultipleHandleSliderHandle::moveHandeToPos(int pos)
{
	int absoluteMinSliderPixel = minHandleXPos(false);
	int relativeMinSliderPixel = minHandleXPos(true);
	int absoluteMaxSliderPixel = maxHandleXPos(false);
	int relativeMaxSliderPixel = maxHandleXPos(true);

	// This is for acurate movement from clicked point.
	pos -= mHandleClickXPixel;

	if( pos > relativeMaxSliderPixel )
		pos = relativeMaxSliderPixel;
	else
	if( pos < relativeMinSliderPixel )
		pos = relativeMinSliderPixel;
	// This both are the same formula but second one is reduced.
//	float percentage = (100.0 * handlePossition.x()) / float(maxSliderPixel-minSliderPixel);
//	newValue = int(percentage * (parentSlider()->maximum() - parentSlider()->minimum()) / (100.0)) + parentSlider()->minimum();
	int newValue = ((pos-absoluteMinSliderPixel) * (parentSlider()->maximum() - parentSlider()->minimum()) / (absoluteMaxSliderPixel-absoluteMinSliderPixel)) + parentSlider()->minimum() ;

	newValue = checkValidValue(newValue);
	move( pos, handleYPos() );
	if( newValue != mValue )
	{
		mValue = newValue;
		emit valueChanged( value(), handleID() );
	}
}

void QMultipleHandleSliderHandle::updateHandlePos_fromValue()
{
	int minSliderPixel = minHandleXPos(false);
	int maxSliderPixel = maxHandleXPos(false);

	int location = (((mValue-parentSlider()->minimum()) * (maxSliderPixel-minSliderPixel)) / (parentSlider()->maximum() - parentSlider()->minimum())) + minSliderPixel;

	move( location, handleYPos() );
}

void QMultipleHandleSliderHandle::onSliderSizeChange(const QSize &newSize, const QSize &oldSize)
{
	Q_UNUSED(newSize);
	Q_UNUSED(oldSize);

	updateHandlePos_fromValue();
}

void QMultipleHandleSliderHandle::setNormalColor()
{
	setPixmap(mNormalPixmap);
}

void QMultipleHandleSliderHandle::setHoverColor()
{
	setPixmap(mHoverPixmap);
}

void QMultipleHandleSliderHandle::drawHandle(QPixmap &pixmap, bool rounded, const QColor &clr)
{
	QPainter p(&pixmap);

	p.setRenderHint(QPainter::Antialiasing);

	if( rounded )
	{
		QPainterPath path;
		path.addRoundedRect( QRectF(0, 0, pixmap.width(), pixmap.height()), 5, 5);
		QPen pen(Qt::black, 1);
		p.setPen(pen);
		p.fillPath(path, clr);
		p.drawPath(path);
	}
	else
	{
		p.fillRect(0, 0, pixmap.width(), pixmap.height(), QBrush(clr) );
	}
}

void QMultipleHandleSliderHandle::createHandlePixmaps(const QColor &normalClr, const QColor &hoverClr, bool rounded)
{
	mNormalPixmap = QPixmap(size());
	mNormalPixmap.fill(Qt::transparent);
	mHoverPixmap = QPixmap(size());
	mHoverPixmap.fill(Qt::transparent);
	drawHandle(mNormalPixmap, rounded, normalClr);
	drawHandle(mHoverPixmap, rounded, hoverClr);
}

bool QMultipleHandleSliderHandle::event(QEvent *ev)
{
	switch( ev->type() )
	{
	case QEvent::Move:
		emit handleMoved();
		return true;
	case QEvent::MouseMove:
		if( handled() )
		{
			handleFollowMouse();
			return true;
		}
		break;
	case QEvent::MouseButtonPress:
		mHandleClickXPixel = mapFromGlobal(QCursor::pos()).x();
		return true;
	case QEvent::MouseButtonRelease:
		mHandleClickXPixel = -1;
		updateHandlePos_fromValue();
		return true;
	case QEvent::Leave:
		setNormalColor();
		return true;
	case QEvent::Enter:
		setHoverColor();
		return true;
	default:
		break;
	}
	return QLabel::event(ev);
}

QMultipleHandleSliderHandle::QMultipleHandleSliderHandle(QMultipleHandleSlider *papi, const QString &id, const QColor &normalClr, const QColor &hoverClr, const QSize &size)
	: QLabel(papi)
	, mHandleID(id)
	, mValue(0)
	, mNormalPixmap(size)
	, mHoverPixmap(size)
	, mHandleClickXPixel(-1)
{
	resize(size);
	createHandlePixmaps(normalClr, hoverClr, true);
	setNormalColor();
}

void QMultipleHandleSliderHandle::setValue(int value)
{
	mValue = value;
	updateHandlePos_fromValue();
}

// To update handlers pos on size changes.
void QMultipleHandleSlider::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	for( QMultipleHandleSliderHandle *handle : mAltHandles )
		handle->onSliderSizeChange(event->oldSize(), event->size());
	if( mMiddleHandle )
		mMiddleHandle->updateView();
}

void QMultipleHandleSlider::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setPen( QColor(0,0,0,64) );
	p.drawLine( 0, height()/2, width(), height()/2 );
	p.drawLine( 0, height()/2 - 1, width(), height()/2 - 1 );
	p.drawLine( 0, height()/2 + 1, width(), height()/2 + 1 );
}

void QMultipleHandleSlider::onValueChanged(int value, const QString &id)
{
	Q_UNUSED(value);
	Q_UNUSED(id);
	if( mMiddleHandle )
		mMiddleHandle->updateView();
	emit valueChanged(value, id);
}

int QMultipleHandleSlider::maxHandleXPos(const QMultipleHandleSliderHandle &handle, bool limitedByOthers) const
{
	if( !mMiddleHandle )
		return width() - handle.width();

	if( !limitedByOthers )
	{
		if( &handle == mMiddleHandle->mHandleA )
			return width() - handle.width() - mMiddleHandle->mHandleB->width();
		return width() - handle.width();
	}
	if( &handle == mMiddleHandle->mHandleA )
		return mMiddleHandle->mHandleB->pos().x() - handle.width();
	return width() - handle.width();
}

int QMultipleHandleSlider::minHandleXPos(const QMultipleHandleSliderHandle &handle, bool limitedByOthers) const
{
	if( !mMiddleHandle )
		return 0;

	if( !limitedByOthers )
	{
		if( &handle == mMiddleHandle->mHandleB )
			return mMiddleHandle->mHandleA->width();
		return 0;
	}
	if( &handle == mMiddleHandle->mHandleB )
		return mMiddleHandle->mHandleA->pos().x() + mMiddleHandle->mHandleA->width();
	return 0;
}

int QMultipleHandleSlider::checkValidValue(const QMultipleHandleSliderHandle *handle, int value)
{
	int max = maximum();
	int min = minimum();

	if( mMiddleHandle )
	{
		if( handle == mMiddleHandle->mHandleA )
			max = mMiddleHandle->mHandleB->value()-1;
		else
		if( handle == mMiddleHandle->mHandleB )
			min = mMiddleHandle->mHandleA->value()+1;
	}

	if( value > max )		value = max;
	if( value < min )		value = min;
	return value;
}

QMultipleHandleSlider::QMultipleHandleSlider(QWidget *parent)
	: QFrame(parent)
	, mMiddleHandle(Q_NULLPTR)
{
	setObjectName(QString::fromUtf8("timeSlider"));
	setFrameShape(QFrame::Panel);
	setFrameShadow(QFrame::Sunken);
}

void QMultipleHandleSlider::setRange(const int &min, const int &max, bool handleFollows)
{
	Q_ASSERT( min <= max );
	if( (max != mMaxValue) || (min != mMinValue) )
	{
		int oldMax = mMaxValue;
		int oldMin = mMinValue;
		mMaxValue = max;
		mMinValue = min;
		for( QMultipleHandleSliderHandle *handle : mAltHandles )
		{
			if( (handle->value() > mMaxValue) || (handleFollows && (handle->value() == oldMax)) )
				handle->setValue(mMaxValue);
			else
			if( (handle->value() < mMinValue) || (handleFollows && (handle->value() == oldMin)) )
				handle->setValue(mMinValue);
			else
				handle->updateHandlePos_fromValue();
		}
	}
}

void QMultipleHandleSlider::addHandle(const QString &id, const QColor &normalClr, const QColor &hoverClr, const QSize &size)
{
	Q_ASSERT( !mAltHandles.contains(id) );
	QMultipleHandleSliderHandle *alt_handle = new QMultipleHandleSliderHandle(this, id, normalClr, hoverClr, size);

	alt_handle->move( this->pos().x() + this->width() - alt_handle->width(), this->pos().y() );
	connect( alt_handle, &QMultipleHandleSliderHandle::valueChanged, this, &QMultipleHandleSlider::valueChanged );
	mAltHandles.insert(id, alt_handle);
}

void QMultipleHandleSlider::addMiddleHandle(const QString &idA, const QString &idB, const QColor &normalClr, const QColor &hoverClr)
{
	QMultipleHandleSliderHandle *sA = mAltHandles[idA];
	QMultipleHandleSliderHandle *sB = mAltHandles[idB];

	if( mMiddleHandle )
		mMiddleHandle->deleteLater();
	mMiddleHandle = new QMultipleHandleSliderMiddleHandle(this, QString("%1-%2").arg(idA).arg(idB), normalClr, hoverClr, sA, sB);
	connect( sA, &QMultipleHandleSliderHandle::handleMoved, mMiddleHandle, &QMultipleHandleSliderMiddleHandle::updateView );
	connect( sB, &QMultipleHandleSliderHandle::handleMoved, mMiddleHandle, &QMultipleHandleSliderMiddleHandle::updateView );
	connect( mMiddleHandle, &QMultipleHandleSliderHandle::handleMoved, mMiddleHandle, &QMultipleHandleSliderMiddleHandle::onHandleMoved );
	mMiddleHandle->updateView();
}

int QMultipleHandleSlider::value(const QString &id) const
{
	return mAltHandles[id]->value();
}

void QMultipleHandleSlider::setValue(int value, const QString &id)
{
	mAltHandles[id]->setValue(value > maximum() ? maximum() : value < minimum() ? minimum() : value);
}

void QMultipleHandleSlider::updateHandlePos(QMultipleHandleSliderHandle *handle)
{
	QPoint parentCursorPossition( handle->parentWidget()->mapFromGlobal(QCursor::pos()) );

	QPoint newHandlePossition( parentCursorPossition.x() - handle->width()/2, handle->y() );

	int horBuffer = handle->width();
	bool lessThanMax = mapToParent(newHandlePossition).x() < pos().x() + width() - horBuffer;
	bool greaterThanMin = mapToParent(newHandlePossition).x() > pos().x();

	if( lessThanMax && greaterThanMin )
	{
		handle->move(newHandlePossition);
		emit valueChanged( handle->value(), handle->handleID() );
	}
}


void QMultipleHandleSliderMiddleHandle::onHandleMoved()
{
	if( mHandleA && mHandleB )
	{
		mMoving = true;
		if( mHandleA->pos().x() != pos().x()-mHandleA->width() )
			mHandleA->moveHandeToPos( pos().x()-mHandleA->width() );
		if( mHandleB->pos().x() != pos().x()+width() )
			mHandleB->moveHandeToPos( pos().x()+width() );
		mMoving = false;
	}
}

void QMultipleHandleSliderMiddleHandle::updateView()
{
	if( !mMoving )
	{
		QRect geo;
		geo.setX( mHandleA->pos().x()+mHandleA->width() );
		geo.setY( mHandleA->pos().y()+4 );
		geo.setWidth( mHandleB->pos().x()-geo.x() );
		geo.setHeight( mHandleA->height()-8 );
		resize(geo.size());
		move( geo.x(), geo.y() );
//		qDebug() << geo;
		createHandlePixmaps(mNormalClr, mHoverClr, false);
		setNormalColor();
	}
}
