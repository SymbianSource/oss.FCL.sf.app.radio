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

#ifndef RADIOMAINVIEW_H
#define RADIOMAINVIEW_H

// System includes
#include <QScopedPointer>

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioWindow;
class RadioStationModel;
class HbPushButton;
class RadioUiLoader;
class RadioFrequencyScanner;
class RadioStationCarousel;
class RadioFrequencyStrip;

// Class declaration
class WIDGETS_DLL_EXPORT RadioMainView : public RadioViewBase
    {
    Q_OBJECT

public:

    explicit RadioMainView();
    ~RadioMainView();

    void setScanningMode( bool scanning );

private slots:

    void setFrequencyFromWidget( uint frequency, int reason );
    void setFrequencyFromEngine( uint frequency, int reason );
    void skip( int skipMode );
    void toggleScanning();
    void seekingStarted();
    void updateAudioRoute( bool loudspeaker );

private:

// from base class RadioViewBase

    void init();
    void setOrientation();
    void userAccepted();

private: // data

    QScopedPointer<RadioFrequencyScanner>   mFrequencyScanner;
    RadioStationCarousel*                   mCarousel;
    RadioFrequencyStrip*                    mFrequencyStrip;

    };

#endif // RADIOMAINVIEW_H
