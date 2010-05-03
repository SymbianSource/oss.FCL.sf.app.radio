/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  FM Radio home screen widget plugin
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "fmradiohswidgetplugin.h"
#include "fmradiohswidget.h"

   
/*!
    Initialize plugin for FM Radio home screen widget. Contains necessary information about 
    the fm radio widget that it can be loaded through QT Service Framework.
*/
QObject *FmRadioHsWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new FmRadioHsWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(fmradiohswidgetplugin, FmRadioHsWidgetPlugin)
