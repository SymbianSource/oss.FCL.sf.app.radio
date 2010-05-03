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
class RadioUiEngine;

class RadioMonitorService : public QObject
{
    Q_OBJECT

public:

    RadioMonitorService( RadioUiEngine& engine );
    virtual ~RadioMonitorService();

    void notifyFavoriteCount( const int favoriteCount );
    void notifyAntennaStatus( bool connected );
    void notifyRadioStatus( RadioStatus::Status radioStatus );
    void notifyFrequency( const uint frequency );
    void notifyName( const QString& name );
    void notifyGenre( const QString& genre );
    void notifyRadioText( const QString& radioText );
    void notifyHomePage( const QString& homePage );
    void notifySong( const QString& song );

private:
    
private: // data

    RadioUiEngine&  mUiEngine;

};

#endif // RADIOMONITORSERVICE_H
