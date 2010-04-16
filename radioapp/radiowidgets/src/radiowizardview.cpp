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
#include <HbListView>
#include <HbAction>

// User includes
#include "radiowizardview.h"
#include "radiobannerlabel.h"
#include "radiomainwindow.h"
#include "radiouiengine.h"
#include "radiolocalization.h"
#include "radiologger.h"
#include "radiostationmodel.h"
#include "radiofrequencyscanner.h"
#include "radioxmluiloader.h"

/*!
 *
 */
RadioWizardView::RadioWizardView( RadioXmlUiLoader* uiLoader ) :
    RadioViewBase( uiLoader ),
    mBannerLabel( 0 ),
    mStartScanningRequested( false )
{
}

/*!
 * Private slot
 *
 */
void RadioWizardView::engineStatusChanged( bool radioIsOn )
{
    if ( radioIsOn )
    {
        LOG("Starting scanning after engine started up");
        disconnect( &mMainWindow->uiEngine(), SIGNAL(radioStatusChanged(bool)), this, SLOT(engineStatusChanged(bool)) );
        startScanning();
    }
}

/*!
 * Private slot
 *
 */
void RadioWizardView::saveSelectedAsFavorites()
{
    mModel->setFavorites( mList->selectionModel()->selectedIndexes() );
    mMainWindow->activateTuningView();
}

/*!
 * Private slot
 *
 */
void RadioWizardView::listItemClicked( const QModelIndex& index )
{
    RadioStation station = mModel->stationAt( index.row() );
    RADIO_ASSERT( station.isValid() , "FMRadio", "invalid RadioStation");

    LOG_FORMAT( "RadioWizardView::setFrequency, selectedRow: %d", station.presetIndex() );

    mMainWindow->uiEngine().tunePreset( station.presetIndex() );
}

/*!
 * From RadioViewBase
 *
 */
void RadioWizardView::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    LOG_METHOD;
    mMainWindow = aMainWindow;
    mModel = aModel;
    mModel->setDetail( RadioStationModel::ShowGenre );

    mBannerLabel        = mUiLoader->findObject<RadioBannerLabel>( DOCML_NAME_WV_HEADINGBANNER );
    mList               = mUiLoader->findWidget<HbListView>( DOCML_NAME_WV_STATIONSLIST );

    mList->setSelectionMode( HbListView::MultiSelection );
    mList->setModel( mModel );
    mList->setScrollingStyle( HbListView::PanOrFlick );

    // "Go to stations view" menu item
    connectViewChangeMenuItem( DOCML_NAME_WV_STATIONSVIEWACTION, SLOT(activateStationsView()) );
    
    setDoneAction();
}

/*!
 * From QGraphicsWidget
 *
 */
void RadioWizardView::showEvent( QShowEvent* event )
{
    RadioViewBase::showEvent( event );

    RadioUiEngine* engine = &mMainWindow->uiEngine();
    if ( engine->isRadioOn() && !mStartScanningRequested )
    {
        LOG("Engine was already running. Starting scanning immediately");
        startScanning();
    }
    else
    {
        connectAndTest( engine, SIGNAL(radioStatusChanged(bool)), this, SLOT(engineStatusChanged(bool)) );
    }
}

/*!
 *
 */
void RadioWizardView::setDoneAction()
{
    // The default back button activates the tuning view
    HbAction* doneAction = new HbAction( Hb::DoneNaviAction, this );
    connectAndTest( doneAction, SIGNAL(triggered()),
                    this,       SLOT(saveSelectedAsFavorites()) );
    setNavigationAction( doneAction );
}

/*!
 *
 */
void RadioWizardView::startScanning()
{
    if ( !mStartScanningRequested ) {
        mStartScanningRequested = true;
        RadioFrequencyScanner* scanner = new RadioFrequencyScanner( mMainWindow->uiEngine(), this );
        scanner->startScanning();
    }
}
