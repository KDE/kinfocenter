/*
 *  SPDX-FileCopyrightText: 2008 Ivo Anjo <knuckles@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCONTROL_OPENGL_H
#define KCONTROL_OPENGL_H

#include <KCModule>

#include "ui_opengl.h"

class KCMOpenGL : public KCModule, public Ui::OpenGL
{
    Q_OBJECT

public:
    explicit KCMOpenGL(QWidget *parent = 0, const QVariantList &list = QVariantList());

protected Q_SLOTS:
    virtual void treeWidgetChanged();
};

#endif
