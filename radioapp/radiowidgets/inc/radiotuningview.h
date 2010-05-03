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

#ifndef _TUNINGVIEW_H_
#define _TUNINGVIEW_H_

// System includes

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioMainWindow;
class RadioStationModel;
class HbPushButton;
class RadioXmlUiLoader;
class RadioFrequencyScanner;
class RadioStationCarousel;

// Class declaration
class WIDGETS_DLL_EXPORT RadioTuningView : public RadioViewBase
    {
    Q_OBJECT

public:

    explicit RadioTuningView();

    void setScanningMode( bool scanning );

private slots:

    void toggleFavorite();
    void startScanning();
    void scanningFinished();
    void seekingStarted();
    void seekingFinished();
    void updateAntennaStatus( bool connected );
    void updateAudioRoute( bool loudspeaker );
    void buttonPressed();
    void removeInfoText();

private:

// from base class RadioViewBase

    void init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow );
    void setOrientation();

// New functions

private: // data

    RadioFrequencyScanner*        mFrequencyScanner;
    RadioStationCarousel*         mCarousel;

    };

#endif // _TUNINGVIEW_H_
