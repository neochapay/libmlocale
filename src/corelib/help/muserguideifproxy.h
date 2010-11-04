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

/*
 * This file was generated by m-servicefwgen
 * Command line was: m-servicefwgen -c MUserGuideIfProxy -p muserguideifproxy MUserGuideIf-24539.xml
 *
 * m-servicefwgen is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef MUSERGUIDEIFPROXY_H_1286290176
#define MUSERGUIDEIFPROXY_H_1286290176

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include <mexport.h>

/*
 * Proxy class for interface com.nokia.UserGuideIf
 */
class M_EXPORT MUserGuideIfProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.nokia.UserGuideIf"; }

public:
    MUserGuideIfProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~MUserGuideIfProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> mainPage()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("mainPage"), argumentList);
    }

    inline QDBusPendingReply<bool> pageByPath(const QString &path)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(path);
        return asyncCallWithArgumentList(QLatin1String("pageByPath"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace nokia {
    typedef ::MUserGuideIfProxy UserGuideIf;
  }
}
#endif
