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

// Class declaration
class WIDGETS_DLL_EXPORT RadioTuningView : public RadioViewBase
    {
    Q_OBJECT

public:

    explicit RadioTuningView( RadioXmlUiLoader* uiLoader );

private slots:

    void toggleFavorite();

private:

// from base class RadioViewBase

    void init( RadioMainWindow* aMainWindow, RadioStationModel* aModel );
    void setOrientation();

// New functions

private: // data

    };

#endif // _TUNINGVIEW_H_
