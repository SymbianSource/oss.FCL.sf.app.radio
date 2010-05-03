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

#include <HbInstance>
#include <HbAction>
#include <HbMessageBox>
#include <HbVolumeSliderPopup>

#include "radiomainwindow.h"
#include "radiotuningview.h"
#include "radiostationsview.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"
#include "radioxmluiloader.h"

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

    HbMessageBox box( HbMessageBox::MessageTypeQuestion );
    box.setText( hbTrId( "txt_rad_info_activate_radio_in_offline_mode" ) );
    box.setTimeout( HbPopup::NoTimeout );
    box.setDismissPolicy( HbPopup::NoDismiss );

    HbAction* primaryAction = new HbAction( hbTrId( "txt_common_button_yes" ) );
    box.setPrimaryAction( primaryAction );
    HbAction* secondaryAction = new HbAction( hbTrId( "txt_common_button_no" ) );
    box.setSecondaryAction( secondaryAction );

    const bool answer = box.exec() == box.primaryAction();

    dialogShown( dummyView );

    return answer;
}

/*!
 *
 */
void RadioMainWindow::showErrorMessage( const QString& text )
{
    DummyViewPtr dummyView = prepareToShowDialog();

    HbMessageBox::warning( text );

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
    connectAndTest( mUiEngine,  SIGNAL(antennaStatusChanged(bool)),
                    this,       SLOT(updateAntennaStatus(bool)) );

    activateTuningView();
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
    return orientation() == Qt::Vertical ? DOCML::SECTION_PORTRAIT : DOCML::SECTION_LANDSCAPE;
}

/*!
 *
 */
void RadioMainWindow::activateTuningView()
{
    activateView( mTuningView, DOCML::FILE_TUNINGVIEW, Hb::ViewSwitchUseBackAnim );
}

/*!
 *
 */
void RadioMainWindow::activateStationsView()
{
    activateView( mStationsView, DOCML::FILE_STATIONSVIEW );
}

/*!
 *
 */
void RadioMainWindow::activateHistoryView()
{
    activateView( mHistoryView, DOCML::FILE_HISTORYVIEW );
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
    if ( !mVolSlider ) {
        mVolSlider.reset( new HbVolumeSliderPopup() );
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
void RadioMainWindow::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        HbMessageBox infoBox( hbTrId( "txt_rad_dpophead_connect_wired_headset" ) );
        infoBox.exec();
    }
}

/*!
 *
 */
void RadioMainWindow::activateView( ViewPtr& aMember, const QString& docmlFile, Hb::ViewSwitchFlags flags )
{
    LOG_METHOD;
    if ( aMember && aMember == currentView() ) {
        return;
    }

    RadioViewBase* previousView = static_cast<RadioViewBase*>( currentView() );
    if ( previousView && previousView->isTransient() ) {
        removeView( previousView );
        previousView->deleteLater();
    }

    bool viewCreated = false;
    if ( !aMember ) {
        viewCreated = true;

        RadioXmlUiLoader* uiLoader = new RadioXmlUiLoader();
        bool ok = false;

        // View takes ownership of the ui loader when it is created inside the load function
        QObjectList objectList = uiLoader->load( docmlFile, &ok );

        RADIO_ASSERT( ok , "FMRadio", "invalid DocML file" );
        if ( !ok ) {
            delete uiLoader;
            uiLoader = 0;
            return;
        }

        aMember = ViewPtr( uiLoader->findObject<RadioViewBase>( DOCML::NAME_VIEW ) );

        foreach( QObject* object, objectList ) {
            const QString className = object->metaObject()->className();
            if ( !object->parent() && object != aMember.data() ) {
                object->setParent( aMember.data() );
            }
        }

        aMember->init( uiLoader, this );

        addView( aMember );
    }

    aMember->updateOrientation( orientation(), viewCreated );

    setCurrentView( aMember, true, flags );
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
