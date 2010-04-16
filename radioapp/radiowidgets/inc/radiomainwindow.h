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

#ifndef _RADIOMAINWINDOW_H_
#define _RADIOMAINWINDOW_H_

// System includes
#include <HbMainWindow>
#include <HbEffect>
#include <QPointer>

//#define QT_SHAREDPOINTER_TRACK_POINTERS // Debugging support for QSharedPointer
#include <QSharedPointer>

// User includes
#include "radiowidgetsexport.h"

// Forward declarations
class RadioViewBase;
class RadioUiEngine;
class HbVolumeSliderPopup;

typedef QSharedPointer<HbVolumeSliderPopup> VolumeSliderPtr;

/**
 * QPointer is used to store the views because it tracks the deletion of the object and nulls
 * the reference. Transient views like RadioWizardView and RadioPlayLogView are destroyed after they are closed
 * and QPointer will notice it.
 */
typedef QPointer<RadioViewBase> ViewPtr;

typedef QSharedPointer<HbView> DummyViewPtr;

// Class declaration
class WIDGETS_DLL_EXPORT RadioMainWindow : public HbMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioMainWindow )

public:

    RadioMainWindow( QWidget *parent = 0 );

    ~RadioMainWindow();

    bool isOfflineUsageAllowed();

    void showErrorMessage( const QString& text );

    void init( RadioUiEngine* uiEngine );

    RadioUiEngine& uiEngine();

    QString orientationSection();

public slots:

    void activateTuningView();

    void activateWizardView();

    void activateStationsView();

    void activatePlayLogView();

private slots:

    void updateOrientation( Qt::Orientation orientation );
    void showVolumeLevel( int volume );
    void headsetStatusChanged( bool connected );

private:

// New functions

    void activateView( ViewPtr& aMember, const QString& docmlFile, Hb::ViewSwitchFlags flags = Hb::ViewSwitchDefault );

    DummyViewPtr prepareToShowDialog();
    void dialogShown( DummyViewPtr pointer );

private: // data

    /*!
     * Pointer to the UI engine
     */
    RadioUiEngine*  mUiEngine;

    /**
     * Tuning view.
     * Own.
     */
    ViewPtr         mTuningView;

    /**
     * Stations view
     * Own.
     */
    ViewPtr         mStationsView;

    /**
     * Wizard view
     * Own.
     */
    ViewPtr         mWizardView;

    /**
     * Play log view
     * Own.
     */
    ViewPtr         mPlayLogView;

    /**
     * Pointer to the volume slider
     * Own.
     */
    VolumeSliderPtr mVolSlider;

};


#endif // _RADIOMAINWINDOW_H_
