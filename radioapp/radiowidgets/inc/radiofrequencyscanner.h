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

#ifndef RADIOPRESETSCANNER_H_
#define RADIOPRESETSCANNER_H_

// System includes
#include <QObject>

// Forward declarations
class HbProgressDialog;
class RadioStation;
class RadioUiEngine;

// Class declaration
class RadioFrequencyScanner : public QObject
{
    Q_OBJECT

public:

    RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent = 0 );

    void startScanning();

signals:

    void frequencyScannerFinished();

private slots:

    void updateScanAndSaveProgress( const RadioStation& station );
    void scanAndSavePresetsCancelled();
    void scanAndSavePresetsFinished();

private: // data

    /*!
     * Reference to the Ui engine
     */
    RadioUiEngine&      mUiEngine;

    /**
     * Scanning progress note
     * Own.
     */
    HbProgressDialog*     mScanningProgressNote;

    /**
     * Amount of radio stations found
     */
    uint                mChannelCount;

    /**
     * Low frequency band edge. The lowest valid frequency at the current region
     */
    uint                mMinFrequency;

};

#endif // RADIOPRESETSCANNER_H_
