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

#include <hbinstance.h>
#include <hbaction.h>
#include <hbmessagebox.h>
#include <hbvolumesliderpopup.h>

#include "radiomainwindow.h"
#include "radiotuningview.h"
#include "radiostationsview.h"
#include "radiowizardview.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"
#include "radioxmluiloader.h"
#include "radiolocalization.h"

// Constants

/**
 * Desired amount of delay of volumesliderpopup
 */
const int KVolumeSliderDelay = 5000;

/*!
 *
 */
RadioMainWindow::RadioMainWindow( QWidget* parent ) :
    HbMainWindow( parent ),
    mUiEngine( 0 )
{
}

/*!
 *
 */
RadioMainWindow::~RadioMainWindow()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.
}

/*!
 *
 */
bool RadioMainWindow::isOfflineUsageAllowed()
{
    DummyViewPtr dummyView = prepareToShowDialog();

    const bool answer = HbMessageBox::launchQuestionMessageBox( TRANSLATE( KQueryTextStartOffline ) );
    dialogShown( dummyView );

    return answer;
}

/*!
 *
 */
void RadioMainWindow::showErrorMessage( const QString& text )
{
    DummyViewPtr dummyView = prepareToShowDialog();

    HbMessageBox::launchWarningMessageBox( text );

    dialogShown( dummyView );
}

/*!
 *
 */
void RadioMainWindow::init( RadioUiEngine* uiEngine )
{
    LOG_METHOD;
    mUiEngine = uiEngine;

    // MainWindow is the one that always listens for orientation changes and then delegates
    // the updates to the views
    connectAndTest( hbInstance->allMainWindows().first(),   SIGNAL(orientationChanged(Qt::Orientation)),
                    this,                                   SLOT(updateOrientation(Qt::Orientation)) );

    connectAndTest( mUiEngine,  SIGNAL(volumeChanged(int)),
                    this,       SLOT(showVolumeLevel(int)) );
    connectAndTest( mUiEngine,  SIGNAL(headsetStatusChanged(bool)),
                    this,       SLOT(headsetStatusChanged(bool)) );

    const bool firstTimeStart = mUiEngine->isFirstTimeStart();
    const int rowCount = mUiEngine->model().rowCount();
    if( firstTimeStart && rowCount == 0 ) {
        activateWizardView();
    } else {
        activateTuningView();
    }
}

/*!
 *
 */
RadioUiEngine& RadioMainWindow::uiEngine()
{
    return *mUiEngine;
}

/*!
 * Returns the XML layout section that corresponds to the view orientation
 */
QString RadioMainWindow::orientationSection()
{
    return orientation() == Qt::Vertical ? DOCML_LAYOUT_PORTRAIT : DOCML_LAYOUT_LANDSCAPE;
}

/*!
 *
 */
void RadioMainWindow::activateTuningView()
{
    activateView( mTuningView, DOCML_TUNINGVIEW_FILE );
}

/*!
 *
 */
void RadioMainWindow::activateWizardView()
{
    activateView( mWizardView, DOCML_WIZARDVIEW_FILE );
}

/*!
 *
 */
void RadioMainWindow::activateStationsView()
{
    activateView( mStationsView, DOCML_STATIONSVIEW_FILE );
}

/*!
 *
 */
void RadioMainWindow::activatePlayLogView()
{
    activateView( mPlayLogView, DOCML_PLAYLOGVIEW_FILE );
}

/*!
 * Private slot
 *
 */
void RadioMainWindow::updateOrientation( Qt::Orientation orientation )
{
    HbView* view = currentView();
    RADIO_ASSERT( view, "RadioMainWindow::updateOrientation", "Current view not found!" );
    if ( view ) {
        static_cast<RadioViewBase*>( view )->updateOrientation( orientation );
    }
}

/*!
 * Private slot
 *
 */
void RadioMainWindow::showVolumeLevel( int volume )
{
    if ( !mVolSlider )
    {
        mVolSlider = VolumeSliderPtr( new HbVolumeSliderPopup() );
        mVolSlider->setRange( 0, KMaximumVolumeLevel );
        mVolSlider->setSingleStep( 1 );
        mVolSlider->setTimeout( KVolumeSliderDelay );
        connectAndTest( mVolSlider.data(),  SIGNAL(valueChanged(int)),
                        mUiEngine,          SLOT(setVolume(int)) );
    }

    mVolSlider->setValue( volume );
    mVolSlider->setText( QString( "%L1%" ).arg( volume * 100 / KMaximumVolumeLevel ) );
    mVolSlider->show();
}

/*!
 * Private slot
 *
 */
void RadioMainWindow::headsetStatusChanged( bool connected )
{
    if ( !connected ) {
        HbMessageBox infoBox( TRANSLATE( KTitleHeadsetDisconnectedPopup ) );
        infoBox.exec();
    }
}

/*!
 *
 */
void RadioMainWindow::activateView( ViewPtr& aMember, const QString& docmlFile )
{
    if ( aMember && aMember == currentView() ) {
        return;
    }

    // Remove the secondary softkey of the previous view
    //TODO: Check how the soft keys work nowaways in Orbit. This doesn't seem to do anything
    RadioViewBase* previousView = static_cast<RadioViewBase*>( currentView() );
    if ( previousView && previousView->secondarySoftkey() ) {
        removeSoftKeyAction( Hb::SecondarySoftKey, previousView->secondarySoftkey() );
        if ( previousView->isTransient() ) {
            removeView( previousView );
            previousView->deleteLater();
        }
    }

    bool viewCreated = false;
    if ( !aMember ) {
        viewCreated = true;

        RadioXmlUiLoader* uiLoader = new RadioXmlUiLoader( *this );
        bool ok = false;

        // View takes ownership of the ui loader when it is created inside the load function
        uiLoader->load( docmlFile, &ok );

        RADIO_ASSERT( ok , "FMRadio", "invalid DocML file" );
        if ( !ok ) {
            delete uiLoader;
            uiLoader = 0;
            return;
        }

        aMember = ViewPtr( uiLoader->findObject<RadioViewBase>( DOCML_NAME_VIEW ) );
        aMember->init( this, &mUiEngine->model() );
        aMember->initSecondarySoftkey();

        addView( aMember );
    }

    // Add the secondary softkey if the view has one
    if ( aMember->secondarySoftkey() ) {
        addSoftKeyAction( Hb::SecondarySoftKey, aMember->secondarySoftkey() );
    }

    aMember->updateOrientation( orientation(), viewCreated );

    setCurrentView( aMember, true );
}

/*!
 *
 */
DummyViewPtr RadioMainWindow::prepareToShowDialog()
{
    // To be able to draw a dialog on screen we need a HbMainWindow instance and a HbView to get a graphics scene
    // so we create a dummy view and set it active
    DummyViewPtr dummyView( new HbView() );
    addView( dummyView.data() );
    setCurrentView( dummyView.data() );
    show();
    return dummyView;
}

/*!
 *
 */
void RadioMainWindow::dialogShown( DummyViewPtr pointer )
{
    // Clean up the dummy view
    hide();
    removeView( pointer.data() );
    pointer.clear();
}
