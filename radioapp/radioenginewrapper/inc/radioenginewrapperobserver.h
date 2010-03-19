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

#ifndef RADIOENGINEWRAPPEROBSERVER_H_
#define RADIOENGINEWRAPPEROBSERVER_H_

#include "radio_global.h"

class RadioEngineWrapperObserver
{
public:

    virtual void tunedToFrequency( uint frequency, int commandSender ) = 0;
    virtual void seekingStarted( Seeking::Direction direction ) = 0;
    virtual void radioStatusChanged( bool radioIsOn ) = 0;

    virtual void rdsAvailabilityChanged( bool available ) = 0;

    virtual void volumeChanged( int volume ) = 0;
    virtual void muteChanged( bool muted ) = 0;

    virtual void audioRouteChanged( bool loudspeaker ) = 0;
    virtual void scanAndSaveFinished() = 0;
    virtual void headsetStatusChanged( bool connected ) = 0;

    virtual void skipPrevious() = 0;
    virtual void skipNext() = 0;

};

#endif // RADIOENGINEWRAPPEROBSERVER_H_
