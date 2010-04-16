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
#include <xqserviceprovider.h>

// User includes

// Forward declarations
class RadioUiEngine;

class RadioMonitorService : public XQServiceProvider
{
    Q_OBJECT

public:

    RadioMonitorService( RadioUiEngine& engine );
    virtual ~RadioMonitorService();

    void notifyName( const QString& name );
    void notifyGenre( const QString& genre );
    void notifyRadioText( const QString& radioText );
    void notifyHomepage( const QString& homepage );
    void notifySong( const QString& song );

public slots:

    void requestNotifications();

private:
    
    void notify( int notificationId, const QString& message );
    
private: // data

    RadioUiEngine&  mUiEngine;
    
    int mRequestIndex;

};

#endif // RADIOMONITORSERVICE_H
