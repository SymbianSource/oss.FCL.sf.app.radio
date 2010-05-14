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

// System includes
#include <HbInstance>
#include <HbAction>
#include <HbDeviceMessageBox>
#include <HbVolumeSliderPopup>

// User includes
#include "radiowindow.h"
#include "radiomainview.h"
#include "radiostationsview.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"
#include "radiouiloader.h"

// Constants

/**
 * Desired amount of delay of volumesliderpopup
 */
const int KVolumeSliderDelay = 5000;

/*!
 *
 */
RadioWindow::RadioWindow( QWidget* parent ) :
    HbMainWindow( parent ),
    mUiEngine( new RadioUiEngine( this ) )
{
}

/*!
 *
 */
RadioWindow::~RadioWindow()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.
}

/*!
 *
 */
void RadioWindow::showErrorMessage( const QString& text )
{
    HbDeviceMessageBox box( text, HbMessageBox::MessageTypeWarning );
    box.setTimeout( HbPopup::NoTimeout );
    box.setDismissPolicy( HbPopup::NoDismiss );
    box.exec();
}

/*!
 *
 */
void RadioWindow::init()
{
    connectAndTest( this, SIGNAL(viewReady()),
                    this, SLOT(initView()) );

    activateMainView();
}

/*!
 *
 */
RadioUiEngine& RadioWindow::uiEngine()
{
    return *mUiEngine;
}

/*!
 * Returns the XML layout section that corresponds to the view orientation
 */
QString RadioWindow::orientationSection()
{
    return orientation() == Qt::Vertical ? DOCML::SECTION_PORTRAIT : DOCML::SECTION_LANDSCAPE;
}

/*!
 *
 */
void RadioWindow::activateMainView()
{
    activateView( mMainView, DOCML::FILE_MAINVIEW, Hb::ViewSwitchUseBackAnim );
}

/*!
 *
 */
void RadioWindow::activateStationsView()
{
    activateView( mStationsView, DOCML::FILE_STATIONSVIEW );
}

/*!
 *
 */
void RadioWindow::activateHistoryView()
{
    activateView( mHistoryView, DOCML::FILE_HISTORYVIEW );
}

/*!
 * Private slot
 *
 */
void RadioWindow::initView()
{
    if ( !mUiEngine->isInitialized() ) {
        // Start the engine
        if ( !mUiEngine->init() ) {
            showErrorMessage( hbTrId( "txt_fmradio_info_fm_radio_could_not_be_started" ) );
            qApp->quit();
            return;
        }

        // MainWindow is the one that always listens for orientation changes and then delegates
        // the updates to the views
        connectAndTest( this,               SIGNAL(orientationChanged(Qt::Orientation)),
                        this,               SLOT(updateOrientation(Qt::Orientation)) );

        connectAndTest( mUiEngine.data(),   SIGNAL(volumeChanged(int)),
                        this,               SLOT(showVolumeLevel(int)) );
        connectAndTest( mUiEngine.data(),   SIGNAL(antennaStatusChanged(bool)),
                        this,               SLOT(updateAntennaStatus(bool)) );
    }

    RadioViewBase* view = static_cast<RadioViewBase*>( currentView() );
    if ( !view->isInitialized() ) {
        view->init();
    }
}

/*!
 * Private slot
 *
 */
void RadioWindow::updateOrientation( Qt::Orientation orientation )
{
    HbView* view = currentView();
    RADIO_ASSERT( view, "RadioWindow::updateOrientation", "Current view not found!" );
    if ( view ) {
        static_cast<RadioViewBase*>( view )->updateOrientation( orientation );
    }
}

/*!
 * Private slot
 *
 */
void RadioWindow::showVolumeLevel( int volume )
{
    if ( !mVolSlider ) {
        mVolSlider.reset( new HbVolumeSliderPopup() );
        mVolSlider->setRange( 0, KMaximumVolumeLevel );
        mVolSlider->setSingleStep( 1 );
        mVolSlider->setTimeout( KVolumeSliderDelay );
        connectAndTest( mVolSlider.data(),  SIGNAL(valueChanged(int)),
                        mUiEngine.data(),   SLOT(setVolume(int)) );
    }

    mVolSlider->setValue( volume );
    mVolSlider->setText( QString( "%L1%" ).arg( volume * 100 / KMaximumVolumeLevel ) );
    mVolSlider->show();
}

/*!
 * Private slot
 *
 */
void RadioWindow::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        HbMessageBox::information( hbTrId( "txt_rad_dpophead_connect_wired_headset" ) );
    }
}

/*!
 *
 */
void RadioWindow::activateView( ViewPtr& aMember, const QString& docmlFile, Hb::ViewSwitchFlags flags )
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

        RadioUiLoader* uiLoader = new RadioUiLoader();
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

        aMember->setMembers( uiLoader, this );

        addView( aMember );
    }

    aMember->updateOrientation( orientation(), viewCreated );

    setCurrentView( aMember, true, flags );
}
