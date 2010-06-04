/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef RADIOMONITORSERVICE_H
#define RADIOMONITORSERVICE_H

// System includes
#include <QObject>

// User includes
#include "radioservicedef.h"

// Forward declarations
class RadioUiEnginePrivate;

class RadioMonitorService : public QObject
{
    Q_OBJECT

public:

    RadioMonitorService( RadioUiEnginePrivate& engine );
    virtual ~RadioMonitorService();

    void init();

    void notifySong( const QString& song );

private:
    
private: // data

    RadioUiEnginePrivate&  mUiEngine;

};

#endif // RADIOMONITORSERVICE_H
