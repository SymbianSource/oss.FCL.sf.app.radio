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

#ifndef RADIOWIZARDVIEW_H_
#define RADIOWIZARDVIEW_H_

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioMainWindow;
class HbLabel;
class HbPushButton;
class HbListView;
class RadioBannerLabel;

// Class declaration
class WIDGETS_DLL_EXPORT RadioWizardView : public RadioViewBase
    {
    Q_OBJECT

public:

    explicit RadioWizardView( RadioXmlUiLoader* uiLoader );

private:

// from base class RadioViewBase

    void init( RadioMainWindow* aMainWindow, RadioStationModel* aModel );

// from base class QGraphicsWidget

    void showEvent( QShowEvent* event );

private slots:

    void saveSelectedAsFavorites();
    void listItemClicked( const QModelIndex& index );
    void engineStatusChanged( bool radioIsOn );

private:

    void setDoneAction();
    void startScanning();

private: // data

    HbListView*        mList;
    RadioBannerLabel*  mBannerLabel;
    bool               mStartScanningRequested;

    };

#endif // RADIOWIZARDVIEW_H_
