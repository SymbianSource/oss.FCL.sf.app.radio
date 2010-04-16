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

#ifndef RADIOUIENGINE_P_H_
#define RADIOUIENGINE_P_H_

// System includes
#include <QScopedPointer>

#include "radioenginewrapperobserver.h"

// Forward declarations
class RadioUiEngine;
class RadioEngineWrapper;
class RadioStationModel;
class RadioPlayLogModel;
class RadioPresetStorage;
class RadioControlService;
class RadioMonitorService;

class RadioUiEnginePrivate : public RadioEngineWrapperObserver
{
    Q_DECLARE_PUBLIC( RadioUiEngine )
    Q_DISABLE_COPY( RadioUiEnginePrivate )

public:

    enum TuneDirection{ Next, Previous };

    RadioUiEnginePrivate( RadioUiEngine* engine );
    ~RadioUiEnginePrivate();

    bool startRadio();

private:

// from base class RadioEngineWrapperObserver

    void tunedToFrequency( uint frequency, int commandSender );
    void seekingStarted( Seeking::Direction direction );
    void radioStatusChanged( bool radioIsOn );
    void rdsAvailabilityChanged( bool available );
    void volumeChanged( int volume );
    void muteChanged( bool muted );
    void audioRouteChanged( bool loudspeaker );
    void scanAndSaveFinished();
    void headsetStatusChanged( bool connected );
    void skipPrevious();
    void skipNext();

// New functions

    /*!
     * Tunes to next or previous favorite preset
     */
    void skip( TuneDirection direction );

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioUiEngine*                      q_ptr;

    QScopedPointer<RadioEngineWrapper>  mEngineWrapper;

    QScopedPointer<RadioPresetStorage>  mPresetStorage;

    RadioStationModel*                  mStationModel;

    RadioPlayLogModel*                  mPlayLogModel;

    RadioControlService*                mControlService;

    RadioMonitorService*                mMonitorService;

};

#endif // RADIOUIENGINE_P_H_
