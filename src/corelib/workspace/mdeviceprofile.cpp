/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QString>
#include <QSize>
#include <QSettings>
#include <QFile>
#include <QDir>

#include "mdebug.h"
#include "mdeviceprofile.h"
#include "mdeviceprofile_p.h"

#include "mapplication.h"
#include "mcomponentdata.h"
#include "mcomponentdata_p.h"

MDeviceProfilePrivate::MDeviceProfilePrivate()
    : q_ptr(0)
{
    QString filename = M_INSTALL_SYSCONFDIR "/meegotouch/devices.conf";

#ifdef Q_OS_WIN
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    appDir.cd("etc");
    appDir.cd("meegotouch");
    filename = appDir.path().append("/devices.conf" );
#endif

    if(!load(filename)) {
        qFatal("Failed to load device profile '%s'", qPrintable(filename));
    }
}

MDeviceProfilePrivate::~MDeviceProfilePrivate()
{
}

bool MDeviceProfilePrivate::load(const QString& filename)
{
    QString device = MApplication::deviceName();
    if(device.isEmpty())
        device = "Default";

    if(!QFile::exists(filename))
        return false;

    QSettings settings(filename, QSettings::IniFormat);
    if(settings.status() != QSettings::NoError)
        return false;

    resolution.setWidth(settings.value(device + "/resolutionX", 0).toInt());
    resolution.setHeight(settings.value(device + "/resolutionY", 0).toInt());
    pixelsPerInch.setWidth(settings.value(device + "/ppiX", 0).toInt());
    pixelsPerInch.setHeight(settings.value(device + "/ppiY", 0).toInt());
    showStatusBar=settings.value(device+"/showStatusBar",false).toBool();

    QStringList orientationsForDevOpen = settings.value(device + "/allowedOrientationsOnKeyboardOpen",
                                           QStringList()).toStringList();
    if (orientationsForDevOpen.contains("0"))
        supportedOrientationsForKeyboardOpen << M::Angle0;
    if (orientationsForDevOpen.contains("90"))
        supportedOrientationsForKeyboardOpen << M::Angle90;
    if (orientationsForDevOpen.contains("180"))
        supportedOrientationsForKeyboardOpen << M::Angle180;
    if (orientationsForDevOpen.contains("270"))
        supportedOrientationsForKeyboardOpen << M::Angle270;

    QStringList orientationsForDevClosed = settings.value(device + "/allowedOrientationsOnKeyboardClosed",
                                           QStringList()).toStringList();
    if (orientationsForDevClosed.contains("0"))
        supportedOrientationsForKeyboardClosed << M::Angle0;
    if (orientationsForDevClosed.contains("90"))
        supportedOrientationsForKeyboardClosed << M::Angle90;
    if (orientationsForDevClosed.contains("180"))
        supportedOrientationsForKeyboardClosed << M::Angle180;
    if (orientationsForDevClosed.contains("270"))
        supportedOrientationsForKeyboardClosed << M::Angle270;

    return true;
}

/*
 * @return pointer to the singleton MDeviceProfile instance
 */
MDeviceProfile *MDeviceProfile::instance()
{
    MComponentData *data = MComponentData::instance();
    if (!data)
    {
        qFatal("There is no instance of MDeviceProfile. Please create MComponentData first.");
        return 0;
    }
    else
    {
        return data->d_ptr->deviceProfile;
    }
}

MDeviceProfile::MDeviceProfile(QObject *parent)
    : QObject(parent),
      d_ptr(new MDeviceProfilePrivate)
{
    Q_D(MDeviceProfile);
    d->q_ptr = this;

    if (!MComponentData::instance())
        qFatal("There is no instance of MComponentData. Please create MApplication first.");
    if (MComponentData::instance()->d_ptr->deviceProfile)
        qFatal("Device profile is already created. Please use MDeviceProfile::instance()");
}

MDeviceProfile::~MDeviceProfile()
{
    delete d_ptr;
}

QSize MDeviceProfile::resolution() const
{
    Q_D(const MDeviceProfile);
    return d->resolution;
}

QSize MDeviceProfile::pixelsPerInch() const
{
    Q_D(const MDeviceProfile);
    return d->pixelsPerInch;
}

bool MDeviceProfile::showStatusbar() const
{
    Q_D(const MDeviceProfile);
    return d->showStatusBar;
}

bool MDeviceProfile::orientationAngleIsSupported(M::OrientationAngle angle, bool isKeyboardOpen) const
{
    Q_D(const MDeviceProfile);
    if (isKeyboardOpen)
        return d->supportedOrientationsForKeyboardOpen.contains(angle);
    else
        return d->supportedOrientationsForKeyboardClosed.contains(angle);
}
