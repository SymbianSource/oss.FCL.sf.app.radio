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

#ifndef RADIOFREQUENCYSCANNINGHANDLER_H
#define RADIOFREQUENCYSCANNINGHANDLER_H

// System includes
#include <QObject>
#include <QMap>

// User includes
#include "mradioscanobserver.h"

// Forward declarations
class RadioEngineWrapperPrivate;

// Constants

// Class declaration
class RadioFrequencyScanningHandler : public QObject
                                    , public MRadioScanObserver
    {
    Q_OBJECT
public:

    explicit RadioFrequencyScanningHandler( RadioEngineWrapperPrivate& radioEngine );

// from base class MRadioScanObserver

    void ScanFrequencyEventL( TUint32 aFrequency );
    void ScanCompletedEventL( TInt aError );

// New functions

    /*!
     * Starts the scanning
     * @param muted flag to indicate if the audio was already muted when scanning began
     */
    void startScanning( bool muted );

    /*!
     * Adds a new station that was found
     * @param frequency Frequency of the found station
     */
    void addFoundStation( const uint frequency );

    /*!
     * Cancels the scanning process
     */
    void cancel();

private slots:

    void delayedStart();

private: // data

    /**
     * Reference to the engine
     */
    RadioEngineWrapperPrivate&  mEngine;

    /**
     * Last scanned frequency. Used to know when the scanning has looped around the
     * frequency band and it should stop
     */
    uint                        mLastFoundFrequency;
    };

#endif // RADIOFREQUENCYSCANNINGHANDLER_H
