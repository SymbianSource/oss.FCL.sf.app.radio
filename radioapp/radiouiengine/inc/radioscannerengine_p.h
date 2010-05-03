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

#ifndef RADIOSCANNERENGINE_P_H_
#define RADIOSCANNERENGINE_P_H_

#include <QObject>
#include <QMap>

// User includes
#include "radioenginewrapperobserver.h"

// Forward declarations
class RadioScannerEngine;
class RadioUiEnginePrivate;
class RadioStationHandlerIf;
class RadioStation;
class RadioEngineWrapper;

// Constants

// Class declaration
class RadioScannerEnginePrivate : public RadioEngineWrapperObserver
    {
    Q_DECLARE_PUBLIC( RadioScannerEngine )
    Q_DISABLE_COPY( RadioScannerEnginePrivate )

public:

    RadioScannerEnginePrivate( RadioScannerEngine* scanner, RadioUiEnginePrivate& uiEngine );
    ~RadioScannerEnginePrivate();

// New functions

    /*!
     * Starts the scanning from minimum frequency
     */
    void startScanning();

    /*!
     * Continues the scanning upwards from current frequency
     */
    void continueScanning();

    /*!
     * Cancels the scanning process
     */
    void cancel();

    /*!
     * Adds a new station that was found
     * @param frequency Frequency of the found station
     */
    void addScannedFrequency( const uint frequency );

signals:

    void stationFound( const RadioStation& station );

private slots:

    void delayedStart();

private:

// from base class RadioEngineWrapperObserver

    void tunedToFrequency( uint frequency, int reason );

// New functions

    void addFrequencyAndReport( const uint frequency );

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioScannerEngine*     q_ptr;

    RadioUiEnginePrivate&   mUiEngine;

    /**
     * Last scanned frequency. Used to know when the scanning has looped around the
     * frequency band and it should stop
     */
    uint                    mLastFoundFrequency;

    bool                    mMutedByScanner;

    };


#endif // RADIOSCANNERENGINE_P_H_
