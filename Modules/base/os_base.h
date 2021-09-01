/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OS_BASE_H_
#define OS_BASE_H_

#include <QString>
#include <QStringList>

#include <QProcess>
#include <QTextStream>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVersionNumber>

#include "config-infocenter.h"
#include <KLocalizedString>
#include <config-X11.h>

#if HAVE_X11
#include <X11/Xlib.h>
#endif

static const QString Value(int val, int numbers = 1)
{
    return QString::number(val).rightJustified(numbers);
}

static const QString HexStr(unsigned long val, int digits)
{
    QString hexstr;
    int i;
    hexstr = QStringLiteral("0x%1").arg(val, digits, 16 /*=HEX*/);
    for (i = hexstr.length() - 1; i > 0; --i)
        if (hexstr[i] == QLatin1Char(' '))
            hexstr[i] = QLatin1Char('0');
    return hexstr;
}

#if HAVE_X11
static const QString Order(int order)
{
    if (order == LSBFirst)
        return i18n("LSBFirst");
    else if (order == MSBFirst)
        return i18n("MSBFirst");
    else
        return i18n("Unknown Order %1", order);
}
#endif

static const QString BitString(unsigned long n)
{
    return i18np("1 Bit", "%1 Bits", n);
}

static const QString ByteString(unsigned long n)
{
    return i18np("1 Byte", "%1 Bytes", n);
}

#if HAVE_X11
static const struct _event_table {
    const char *name;
    long value;
} event_table[] = {{"KeyPressMask", KeyPressMask},
                   {"KeyReleaseMask", KeyReleaseMask},
                   {"ButtonPressMask", ButtonPressMask},
                   {"ButtonReleaseMask", ButtonReleaseMask},
                   {"EnterWindowMask", EnterWindowMask},
                   {"LeaveWindowMask", LeaveWindowMask},
                   {"PointerMotionMask", PointerMotionMask},
                   {"PointerMotionHintMask", PointerMotionHintMask},
                   {"Button1MotionMask", Button1MotionMask},
                   {"Button2MotionMask", Button2MotionMask},
                   {"Button3MotionMask", Button3MotionMask},
                   {"Button4MotionMask", Button4MotionMask},
                   {"Button5MotionMask", Button5MotionMask},
                   {"ButtonMotionMask", ButtonMotionMask},
                   {"KeymapStateMask", KeymapStateMask},
                   {"ExposureMask", ExposureMask},
                   {"VisibilityChangeMask", VisibilityChangeMask},
                   {"StructureNotifyMask", StructureNotifyMask},
                   {"ResizeRedirectMask", ResizeRedirectMask},
                   {"SubstructureNotifyMask", SubstructureNotifyMask},
                   {"SubstructureRedirectMask", SubstructureRedirectMask},
                   {"FocusChangeMask", FocusChangeMask},
                   {"PropertyChangeMask", PropertyChangeMask},
                   {"ColormapChangeMask", ColormapChangeMask},
                   {"OwnerGrabButtonMask", OwnerGrabButtonMask},
                   {0L, 0}};
#endif

/* easier to read with such a define ! */
#define I18N_MAX(txt, in, fm, maxw)                                                                                                                            \
    {                                                                                                                                                          \
        int n = fm.width(txt = in);                                                                                                                            \
        if (n > maxw)                                                                                                                                          \
            maxw = n;                                                                                                                                          \
    }

#define PIXEL_ADD 20 // add x Pixel to multicolumns..
#define HEXDIGITS (sizeof(int) * 8 / 4) /* 4 Bytes = 32 Bits = 8 Hex-Digits */

/* Helper-function to read output from an external program */
static int GetInfo_ReadfromPipe(QTreeWidget *tree, const char *FileName, bool WithEmptyLines = true)
{
    QProcess proc;
    QTreeWidgetItem *olditem = nullptr;
    QString s;

    proc.start(QString::fromLatin1(FileName), QIODevice::ReadOnly);
    if (!proc.waitForFinished()) {
        // Process hanged or did not start
        return 0;
    }

    QTextStream t(&proc);

    while (!t.atEnd()) {
        s = t.readLine();
        if (!WithEmptyLines && s.length() == 0)
            continue;
        QStringList list;
        list << s;
        olditem = new QTreeWidgetItem(tree, list);
    }

    return tree->topLevelItemCount();
}

#endif /*OS_BASE_H_*/
