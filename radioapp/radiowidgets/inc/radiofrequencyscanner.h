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
#include <QScopedPointer>

// Forward declarations
class HbProgressDialog;
class RadioStation;
class RadioUiEngine;
class RadioXmlUiLoader;
class RadioMainWindow;
class RadioFrequencyStrip;
class RadioStationCarousel;
class RadioScannerEngine;

// Class declaration
class RadioFrequencyScanner : public QObject
{
    Q_OBJECT

public:

    RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent );
    ~RadioFrequencyScanner();

    void startScanning( RadioXmlUiLoader& uiLoader );

signals:

    void frequencyScannerFinished();

private slots:

    void delayedStart();
    void updateScanAndSaveProgress( const RadioStation& station );
    void continueScanning();
    void scanAndSavePresetsCancelled();
    void restoreUiControls();

private:

    void scanAndSavePresetsFinished();

private: // data

    /*!
     * Reference to the Ui engine
     */
    RadioUiEngine&      mUiEngine;

    bool                mInTuningView;

    QScopedPointer<RadioScannerEngine> mScannerEngine;

    /**
     * Scanning progress note
     * Own.
     */
    HbProgressDialog*     mScanningProgressNote;

    /**
     * Amount of radio stations found
     */
    uint                mChannelCount;

    int                 mStripScrollTime;
    int                 mCarouselScrollTime;

};

#endif // RADIOPRESETSCANNER_H_
