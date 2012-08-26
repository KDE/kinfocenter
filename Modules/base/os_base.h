
/*
Copyright 2010  Nicolas Ternisien <nicolas.ternisien@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OS_BASE_H_
#define OS_BASE_H_

#include <QString>
#include <QStringList>

#include <QProcess>
#include <QTextStream>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>

#include <X11/Xlib.h>

#include "config-infocenter.h"

static const QString Value(int val, int numbers=1) {
	return KGlobal::locale()->formatNumber(val, 0).rightJustified(numbers);
}

static const QString HexStr(unsigned long val, int digits) {
	QString hexstr;
	int i;
	hexstr = QString::fromLatin1("0x%1").arg(val, digits, 16/*=HEX*/);
	for (i=hexstr.length()-1; i>0; --i)
		if (hexstr[i]==' ')
			hexstr[i] = '0';
	return hexstr;
}

static const QString Order(int order) {
	if (order==LSBFirst)
		return i18n("LSBFirst");
	else if (order==MSBFirst)
		return i18n("MSBFirst");
	else
		return i18n("Unknown Order %1", order);
}

static const QString BitString(unsigned long n) {
	return i18np("1 Bit", "%1 Bits", n);
}

static const QString ByteString(unsigned long n) {
	return i18np("1 Byte", "%1 Bytes", n);
}

static const struct _event_table {
	const char *name;
	long value;
} event_table[] = { { "KeyPressMask", KeyPressMask }, { "KeyReleaseMask", KeyReleaseMask }, { "ButtonPressMask", ButtonPressMask }, { "ButtonReleaseMask", ButtonReleaseMask }, { "EnterWindowMask", EnterWindowMask }, { "LeaveWindowMask", LeaveWindowMask }, { "PointerMotionMask", PointerMotionMask },
		{ "PointerMotionHintMask", PointerMotionHintMask }, { "Button1MotionMask", Button1MotionMask }, { "Button2MotionMask", Button2MotionMask }, { "Button3MotionMask", Button3MotionMask }, { "Button4MotionMask", Button4MotionMask }, { "Button5MotionMask", Button5MotionMask }, {
				"ButtonMotionMask", ButtonMotionMask }, { "KeymapStateMask", KeymapStateMask }, { "ExposureMask", ExposureMask }, { "VisibilityChangeMask", VisibilityChangeMask }, { "StructureNotifyMask", StructureNotifyMask }, { "ResizeRedirectMask", ResizeRedirectMask }, {
				"SubstructureNotifyMask", SubstructureNotifyMask }, { "SubstructureRedirectMask", SubstructureRedirectMask }, { "FocusChangeMask", FocusChangeMask }, { "PropertyChangeMask", PropertyChangeMask }, { "ColormapChangeMask", ColormapChangeMask }, { "OwnerGrabButtonMask",
				OwnerGrabButtonMask }, { 0L, 0 } };

/* easier to read with such a define ! */
#define I18N_MAX(txt,in,fm,maxw) \
    { int n = fm.width(txt=in); if (n>maxw) maxw=n; }

#define PIXEL_ADD	20	// add x Pixel to multicolumns..
#define HEXDIGITS (sizeof(int)*8/4)	/* 4 Bytes = 32 Bits = 8 Hex-Digits */

static QTreeWidgetItem* XServer_fill_screen_info(QTreeWidgetItem *lBox, QTreeWidgetItem *last, Display *dpy, int scr, int default_scr) {
	unsigned width, height;
	double xres, yres;
	int i, ndepths, *depths;
	Screen *s= ScreenOfDisplay(dpy,scr); /* opaque structure */
	QTreeWidgetItem *item;

	/*
	 * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
	 *
	 *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
	 *         = N pixels / (M inch / 25.4)
	 *         = N * 25.4 pixels / M inch
	 */

	xres = ((double)(DisplayWidth(dpy,scr) *25.4)/DisplayWidthMM(dpy,scr) );
	yres = ((double)(DisplayHeight(dpy,scr)*25.4)/DisplayHeightMM(dpy,scr));

	QStringList screenList;
	screenList << ki18n("Screen # %1").subs((int)scr,-1).toString();
	if (scr==default_scr)
		screenList << i18n("(Default Screen)");
	else
		screenList << QString();

	item = new QTreeWidgetItem(lBox, screenList);
	if (scr==default_scr)
		item->setExpanded(true);

	QStringList dimensionList;
	dimensionList << i18n("Dimensions") << i18n("%1 x %2 Pixels (%3 x %4 mm)", (int)DisplayWidth(dpy,scr) , (int)DisplayHeight(dpy,scr) , (int)DisplayWidthMM(dpy,scr) , (int)DisplayHeightMM (dpy,scr) );
	last = new QTreeWidgetItem(item, dimensionList);

	QStringList resolutionList;
	resolutionList << i18n("Resolution") << i18n("%1 x %2 dpi", (int)(xres+0.5) , (int)(yres+0.5) );

	last = new QTreeWidgetItem(item, resolutionList);

	ndepths = 0;
	depths = 0;
	depths = XListDepths(dpy, scr, &ndepths);
	if (depths) {
		QString txt;

		for (i = 0; i < ndepths; i++) {
			txt = txt + Value(depths[i]);
			if (i < ndepths - 1)
				txt = txt + QLatin1String(", ");
		}

		QStringList depthList;
		depthList << ki18n("Depths (%1)").subs(ndepths,-1).toString() << txt;
		last = new QTreeWidgetItem(item, depthList);
		XFree((char *) depths);
	}

	QStringList rootList;
	rootList << i18n("Root Window ID") << HexStr((unsigned long)RootWindow(dpy,scr), HEXDIGITS);
	last = new QTreeWidgetItem(item, rootList);

	QStringList depthRootList;
	depthRootList << i18n("Depth of Root Window") << i18np("%1 plane", "%1 planes", DisplayPlanes(dpy,scr));
	last = new QTreeWidgetItem(item, depthRootList);

	QStringList colormapList;
	colormapList << i18n("Number of Colormaps") << i18n("minimum %1, maximum %2", (int)MinCmapsOfScreen(s), (int)MaxCmapsOfScreen(s));
	last = new QTreeWidgetItem(item, colormapList);

	QStringList defaultColormap;
	defaultColormap << i18n("Default Colormap"), Value((int)DefaultColormap(dpy,scr));
	last = new QTreeWidgetItem(item, defaultColormap);

	QStringList colormapCellList;
	colormapCellList << i18n("Default Number of Colormap Cells") << Value((int)DisplayCells(dpy, scr));
	last = new QTreeWidgetItem(item, colormapCellList);

	QStringList preallocatedList;
	preallocatedList << i18n("Preallocated Pixels") << i18n("Black %1, White %2", BlackPixel(dpy,scr), WhitePixel(dpy,scr));
	last = new QTreeWidgetItem(item, preallocatedList);

	QString YES(i18n("Yes"));
	QString NO(i18n("No"));

	QStringList optionList;
	optionList << i18n("Options") << i18n("backing-store: %1, save-unders: %2", (DoesBackingStore(s) == NotUseful) ? NO : ((DoesBackingStore(s) == Always) ? YES : i18n("When mapped")), 
	DoesSaveUnders(s) ? YES : NO);
	last = new QTreeWidgetItem(item, optionList);

	XQueryBestSize(dpy, CursorShape, RootWindow(dpy,scr), 65535, 65535, &width, &height);
	QStringList largestList;
	largestList << i18n("Largest Cursor");
	if (width == 65535 && height == 65535)
		largestList << i18n("unlimited");
	else
		largestList << QString::fromLatin1("%1 x %2").arg(width).arg(height);
	last = new QTreeWidgetItem(item, largestList);

	QStringList currentInput;
	currentInput << i18n("Current Input Event Mask") << HexStr((unsigned long)EventMaskOfScreen(s), HEXDIGITS);
	last = new QTreeWidgetItem(item, currentInput);
	const struct _event_table *etp;
	for (etp=event_table; etp->name; etp++) {
		if (EventMaskOfScreen(s) & etp->value) {
			QStringList eventList;
			eventList << i18n("Event = %1", HexStr(etp->value, HEXDIGITS)) << etp->name;
			new QTreeWidgetItem(last, eventList);
		}
	}

	if (last->childCount()==0)
		return last;
	else
		return last->child(last->childCount()-1);
}

static bool GetInfo_XServer_Generic(QTreeWidget *lBox) {
	/* Many parts of this source are taken from the X11-program "xdpyinfo" */

	int i, n;
	long req_size;

	Display *dpy;
	XPixmapFormatValues *pmf;

	QString str, txt;
	QTreeWidgetItem *last, *item, *next;

	dpy = XOpenDisplay(0);
	if (!dpy)
		return false;

	QStringList headers;
	headers << i18n("Information") << i18n("Value");
	lBox->setHeaderLabels(headers);
	lBox->setSortingEnabled(false);

	QStringList serverInfo;
	serverInfo << i18n("Server Information");
	next = new QTreeWidgetItem(lBox, serverInfo);
	next->setIcon(0, SmallIcon("xorg"));
	next->setExpanded(true);

	QStringList displayNameList;
	displayNameList << i18n("Name of the Display") << DisplayString(dpy);
	last = new QTreeWidgetItem(next, displayNameList);

	QStringList vendorList;
	vendorList << i18n("Vendor String") << QLatin1String(ServerVendor(dpy));
	last = new QTreeWidgetItem(next, vendorList);

	QStringList vendorReleaseList;
	vendorReleaseList << i18n("Vendor Release Number") << Value((int)VendorRelease(dpy));
	last = new QTreeWidgetItem(next, vendorReleaseList);

	QStringList versionList;
	versionList << i18n("Version Number") << QString::fromLatin1("%1.%2").arg((int)ProtocolVersion(dpy)).arg((int)ProtocolRevision(dpy));
	last = new QTreeWidgetItem(next, versionList);

	QStringList screenList;
	screenList << i18n("Available Screens");
	last = item = new QTreeWidgetItem(next, screenList);
	last->setExpanded(true);
	for (i=0; i<ScreenCount(dpy); i++) {
		item = XServer_fill_screen_info(last, item, dpy, i, (int)DefaultScreen(dpy));
		if (i==0)
			item->setExpanded(true);
	}

	QStringList extensionList;
	extensionList << i18n("Supported Extensions");
	last = new QTreeWidgetItem(next, extensionList );
	item = last;

	int extCount;
	char **extensions = XListExtensions(dpy, &extCount);
	for (i = 0; i < extCount; i++) {
		QStringList extensionInfoList;
		extensionInfoList << QLatin1String(extensions[i]);
		item = new QTreeWidgetItem( last, extensionInfoList );
	}
	XFreeExtensionList(extensions);

	pmf = XListPixmapFormats(dpy, &n);
	QStringList pixmapFormatList;
	pixmapFormatList << i18n("Supported Pixmap Formats");
	last = item = new QTreeWidgetItem(next, pixmapFormatList);

	if (pmf) {

		for (i=0; i<n; i++) {
			QStringList pixmapList;
			pixmapList << i18n("Pixmap Format #%1", i+1) << i18n("%1 BPP, Depth: %2, Scanline padding: %3", pmf[i].bits_per_pixel, BitString(pmf[i].depth), BitString(pmf[i].scanline_pad));

			item = new QTreeWidgetItem(last, pixmapList);
		}
		XFree((char *)pmf);
	}

	req_size = XExtendedMaxRequestSize(dpy);
	if (!req_size)
		req_size = XMaxRequestSize(dpy);

	QStringList requestSizeList;
	requestSizeList << i18n("Maximum Request Size") << ByteString(req_size*4);
	last = new QTreeWidgetItem(next, requestSizeList);

	QStringList motionList;
	motionList << i18n("Motion Buffer Size") << ByteString(XDisplayMotionBufferSize(dpy));
	last = new QTreeWidgetItem(next, motionList);

	QStringList bitmapList;
	bitmapList << i18n("Bitmap");
	last = item = new QTreeWidgetItem(next, bitmapList);

	QStringList unitList;
	unitList << i18n("Unit") << Value(BitmapUnit(dpy));
	item = new QTreeWidgetItem(last, unitList);

	QStringList orderList;
	orderList << i18n("Order") << Order(BitmapBitOrder(dpy));
	item = new QTreeWidgetItem(last, orderList);

	QStringList paddingList;
	paddingList << i18n("Padding") << Value(BitmapPad(dpy));
	item = new QTreeWidgetItem(last, paddingList);

	QStringList imageByteList;
	imageByteList << i18n("Image Byte Order") << Order(ImageByteOrder(dpy));
	last = new QTreeWidgetItem(next, imageByteList);

	XCloseDisplay(dpy);
	return true;
}

/* Helper-function to read output from an external program */
static int GetInfo_ReadfromPipe(QTreeWidget* tree, const char *FileName, bool WithEmptyLines = true) {
	QProcess proc;
	QTreeWidgetItem* olditem= NULL;
	QString s;

	proc.start(FileName, QIODevice::ReadOnly);
	if (!proc.waitForFinished()) {
		// Process hanged or did not start
		return 0;
	}

	QTextStream t(&proc);

	while (!t.atEnd()) {
		s = t.readLine();
		if (!WithEmptyLines && s.length()==0)
			continue;
		QStringList list;
		list << s;
		olditem = new QTreeWidgetItem(tree, list);
	}

	return tree->topLevelItemCount();
}

#endif /*OS_BASE_H_*/
