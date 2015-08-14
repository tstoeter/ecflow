//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "IconProvider.hpp"

#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QPainter>

static IconItem unknownIcon(":/desktop/unknown.svg");
static IconItem linkIcon(":/desktop/link.svg");
static IconItem linkBrokenIcon(":/desktop/link_broken.svg");
static IconItem lockIcon(":/viewer/padlock.svg");
static IconItem warningIcon(":/desktop/warning.svg");
static IconItem errorIcon(":/desktop/error.svg");
static IconItem bookmarkGroupIcon(":/desktop/bookmark_group.svg");
static IconItem embeddedIcon(":/desktop/embedded.svg");

std::map<QString,IconItem*> IconProvider::icons_;

//===========================================
//
// IconItem
//
//===========================================

IconItem::IconItem(QString path) : path_(path)
{
}

QPixmap IconItem::pixmap(int size)
{
 	 std::map<int,QPixmap>::iterator it=pixmaps_.find(size);
	 if(it != pixmaps_.end())
	   	return it->second;
	 else
	 {
	   	QPixmap pix;
		QImageReader imgR(path_);
		if(imgR.canRead())
		{
			imgR.setScaledSize(QSize(size,size));
			QImage img=imgR.read();
			pix=QPixmap::fromImage(img);
		}
		else
		{
		  	pix=unknownIcon.pixmap(size);
		}

		pixmaps_[size]=pix;
		return pix;
	 }
	 return QPixmap();
}

//===========================================
//
// IconProvider
//
//===========================================

IconProvider::IconProvider()
{
}

void IconProvider::add(QString path,QString name)
{
	std::map<QString,IconItem*>::iterator it=icons_.find(name);
	if(it == icons_.end())
	{
		IconItem *p=new IconItem(path);
		icons_[name]=p;
	}
}

IconItem* IconProvider::icon(QString name)
{
	std::map<QString,IconItem*>::iterator it=icons_.find(name);
	if(it != icons_.end())
		return it->second;

	return &unknownIcon;
}

QPixmap IconProvider::pixmap(QString name,int size)
{
	return icon(name)->pixmap(size);
}

QPixmap IconProvider::lockPixmap(int size)
{
	 return lockIcon.pixmap(size);
}

QPixmap IconProvider::warningPixmap(int size)
{
	 return warningIcon.pixmap(size);
}

QPixmap IconProvider::errorPixmap(int size)
{
	 return errorIcon.pixmap(size);
}

static IconProvider	iconProvider;
