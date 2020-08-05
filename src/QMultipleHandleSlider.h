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
#ifndef QMULTIPLEHANDLESLIDER_H
#define QMULTIPLEHANDLESLIDER_H

#include <QMap>
#include <QLabel>
#include <QFrame>

class QMultipleHandleSlider;

class QMultipleHandleSliderHandle: public QLabel
{
	Q_OBJECT

	QString mHandleID;
	int mValue;
	QPixmap mNormalPixmap;
	QPixmap mHoverPixmap;
	int mHandleClickXPixel;

	void handleFollowMouse();

	bool handled() const	{ return (mHandleClickXPixel >= 0) && (mHandleClickXPixel <= width()); }

protected:
	void setNormalColor();
	void setHoverColor();
	void createHandlePixmaps(const QColor &normalClr, const QColor &hoverClr, bool rounded);
	virtual bool event(QEvent *ev) override;
	static void drawHandle(QPixmap &pixmap, bool rounded, const QColor &clr);

	virtual int minHandleXPos(bool limitedByOthers) const;
	virtual int maxHandleXPos(bool limitedByOthers) const;
	int handleYPos();

	int checkValidValue(int value);
public:
	void moveHandeToPos(int pos);

	QMultipleHandleSliderHandle(QMultipleHandleSlider *papi, const QString &id, const QColor &normalClr, const QColor &hoverClr, const QSize &size);
	void updateHandlePos_fromValue();
	void onSliderSizeChange(const QSize &newSize, const QSize &oldSize);

	QMultipleHandleSlider *parentSlider()				{ return qobject_cast<QMultipleHandleSlider*>(parent());	}
	const QMultipleHandleSlider *parentSlider() const	{ return qobject_cast<QMultipleHandleSlider*>(parent());	}

	const QString &handleID() const		{ return mHandleID;	}

	int value() const	{ return mValue;	}
	void setValue(int value);

signals:
	void valueChanged(int value, const QString &id);
	void handleMoved();

	friend class QMultipleHandleSlider;
};

class QMultipleHandleSliderMiddleHandle : public QMultipleHandleSliderHandle
{
	Q_OBJECT

	QMultipleHandleSliderHandle *mHandleA;
	QMultipleHandleSliderHandle *mHandleB;

	QColor mNormalClr;
	QColor mHoverClr;

	bool mMoving;
public:
	void onHandleMoved();
	QMultipleHandleSliderMiddleHandle(QMultipleHandleSlider *papi, const QString &id, const QColor &normalClr, const QColor &hoverClr, QMultipleHandleSliderHandle *handleA = Q_NULLPTR, QMultipleHandleSliderHandle *handleB = Q_NULLPTR)
		: QMultipleHandleSliderHandle(papi, id, normalClr, hoverClr, QSize(10,10) )
		, mHandleA(handleA)
		, mHandleB(handleB)
		, mNormalClr(normalClr)
		, mHoverClr(hoverClr)
		, mMoving(false)
	{	}
	void setHandles( QMultipleHandleSliderHandle *a, QMultipleHandleSliderHandle *b )	{ mHandleA = a; mHandleB = b;	}

	void updateView();
	friend class QMultipleHandleSlider;
};

class QMultipleHandleSlider: public QFrame
{
	Q_OBJECT
	QMap<QString, QMultipleHandleSliderHandle*> mAltHandles;
	QMultipleHandleSliderMiddleHandle *mMiddleHandle;
	int mMaxValue;
	int mMinValue;

protected:
	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;

	void onValueChanged(int value, const QString &id);

	int maxHandleXPos(const QMultipleHandleSliderHandle &handle, bool limitedByOthers)const;
	int minHandleXPos(const QMultipleHandleSliderHandle &handle, bool limitedByOthers)const;

	int checkValidValue(const QMultipleHandleSliderHandle *handle, int value);

public:
	QMultipleHandleSlider(QWidget *parent = 0);

	void setRange( const int &min, const int &max, bool handleFollows );
	void setMaximum( const int &max, bool handleFollows )	{ setRange(mMinValue, max, handleFollows);	}
	void setMinimum( const int &min, bool handleFollows )	{ setRange(min, mMaxValue, handleFollows);	}
	const int &maximum( ) const { return mMaxValue;	}
	const int &minimum( ) const { return mMinValue; }

	void addHandle(const QString &id, const QColor &normalClr, const QColor &hoverClr, const QSize &size);
	void addMiddleHandle(const QString &idA, const QString &idB, const QColor &normalClr, const QColor &hoverClr);

	void updateHandlePos(QMultipleHandleSliderHandle *handle);

	int value(const QString &id) const;
	void setValue(int value, const QString &id = QString());

signals:
	void valueChanged(int, const QString &id = "");

	friend class QMultipleHandleSliderHandle;
};

#endif // QMULTIPLEHANDLESLIDER_H
