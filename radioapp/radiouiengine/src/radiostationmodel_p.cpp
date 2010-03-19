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
#include <qtimer>

// User includes
#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiologger.h"
#include "radiopresetstorage.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radioplaylogmodel.h"

// Constants
/**
 * Timeout period for checking if station is sending dynamic PS in milliseconds
 */
const int KDynamicPsCheckTimeout = 10 * 1000;

/*!
 *
 */
RadioStationModelPrivate::RadioStationModelPrivate( RadioStationModel* model,
                                                    RadioUiEngine& uiEngine ) :
    q_ptr( model ),
    mUiEngine( uiEngine ),
    mCurrentStation( &mManualStation ),
    mDynamicPsTimer( new QTimer() )
{
    connectAndTest( mDynamicPsTimer.data(), SIGNAL(timeout()),
                    q_ptr,                  SLOT(dynamicPsCheckEnded()) );
    mDynamicPsTimer->setInterval( KDynamicPsCheckTimeout );
    mDynamicPsTimer->setSingleShot( true );
}

/*!
 *
 */
RadioStationModelPrivate::~RadioStationModelPrivate()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.
}

/*!
 * \reimp
 */
uint RadioStationModelPrivate::currentFrequency() const
{
    return mCurrentStation->frequency();
}

/*!
 * \reimp
 */
int RadioStationModelPrivate::currentPresetIndex() const
{
    return mCurrentStation->presetIndex();
}

/*!
 * Sets the currently tuned frequency. Meant to be set by the engine wrapper
 */
void RadioStationModelPrivate::setCurrentStation( uint frequency )
{
    LOG_METHOD;
    if ( mStations.contains( frequency ) ) {
        // We have to be careful to check that key exists before using operator[]
        // with QMap since it will insert a default-constructed value if it doesn't exist yet.
        mCurrentStation = &mStations[ frequency ];
    } else {
        mManualStation.reset();
        mManualStation.setFrequency( frequency );
        mCurrentStation = &mManualStation;
    }
}

/*!
 * \reimp
 * Sets the genre to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentGenre( uint frequency, int genre )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findCurrentStation( frequency );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
    station.setGenre( genre );
    q->saveStation( station );
}

/*!
 * \reimp
 * Checks if the given frequency exists in the list
 */
bool RadioStationModelPrivate::containsFrequency( uint frequency )
{
    return mStations.contains( frequency );
}

/*!
 * \reimp
 * Checks if the given preset index exists in the list
 */
bool RadioStationModelPrivate::containsPresetIndex( int presetIndex )
{
    Q_Q( RadioStationModel );
    return q->findPresetIndex( presetIndex ) != RadioStation::NotFound;
}

/*!
 * \reimp
 * Starts the dynamic PS check
 */
void RadioStationModelPrivate::startDynamicPsCheck()
{
    // Start the Dynamic PS check if the station has no name and ps type is not known
    // During the dynamic PS check the RadioStation's dynamicPs variable is used to store the
    // received PS name even though at this point it isn't known if it is dynamic or not.
    mDynamicPsTimer->stop();
    if ( mCurrentStation->psType() == RadioStation::Unknown ) {
        mCurrentStation->setDynamicPsText( "" );
        mDynamicPsTimer->start();
    }
}

/*!
 * \reimp
 *
 */
void RadioStationModelPrivate::addScannedFrequency( uint frequency )
{
    Q_Q( RadioStationModel );
    RadioStation station;
    if ( q->findFrequency( frequency, station ) ) {
        station.setType( RadioStation::LocalStation );
        q->saveStation( station );
    } else {
        station.setType( RadioStation::LocalStation );
        station.setFrequency( frequency );
        q->addStation( station );
    }
}

/*!
 * \reimp
 * Removes all local stations that are not favorites
 */
void RadioStationModelPrivate::removeLocalStations()
{
    Q_Q( RadioStationModel );
    foreach( const RadioStation& station, mStations ) {
        if ( station.isType( RadioStation::LocalStation ) && !station.isFavorite() ) {
            q->removeStation( station );
        }
    }
}

/*!
 * \reimp
 * Sets the PS name to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentPsName( uint frequency, const QString& name )
{
    Q_Q( RadioStationModel );
    LOG_FORMAT( "RadioEngineWrapperPrivate::updateCurrentStationName: %s", GETSTRING( name ) );
    RadioStation station = q->findCurrentStation( frequency );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }

    if ( station.psType() == RadioStation::Static ) {

        if ( name.compare( station.name() ) != 0 && !station.isRenamed() ) {
            station.setName( name );
            q->saveStation( station );
        }

    } else {

        if ( mDynamicPsTimer->isActive() ) {    // Dynamic PS check is ongoing
            LOG( "Dynamic Ps check ongoing" );

            if ( !station.dynamicPsText().isEmpty() &&
                    name.compare( station.dynamicPsText(), Qt::CaseInsensitive ) != 0  ) {
                LOG( "Dynamic Ps check - Second PS name arrived and is different. PS is dynamic" );
                station.setPsType( RadioStation::Dynamic ); // Station is sending Dynamic PS
                mDynamicPsTimer->stop();

                // Cleanup the station name if region is not America
                if ( !station.name().isEmpty()
                     && !station.isRenamed()
                     && mEngine->region() != RadioRegion::America )
                {
                    LOG( "Station name cleanup" );
                    station.setName( "" );
                }
            }

            // Received PS name is published to the UI as dynamic PS while the check is ongoing
            // even though at this stage we don't know if it is dynamic or not.

            station.setDynamicPsText( name );
            q->saveStation( station );

        } else {

            if ( station.psType() == RadioStation::Dynamic ) {
                LOG( "Station uses Dynamic Ps" );
            } else {
                LOG( "Station PS type unknown" );
            }

            station.setDynamicPsText( name );
            q->saveStation( station );
        }
    }
}

/*!
 * \reimp
 * Sets the radio text to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentRadioText( uint frequency, const QString& radioText )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findCurrentStation( frequency );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
    station.setRadioText( radioText );
    q->saveStation( station );
    mUiEngine.playLogModel().clearRadioTextPlus();
}

/*!
 * \reimp
 * Sets the radio text plus to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentRadioTextPlus( uint frequency, int rtClass, const QString& rtItem )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findCurrentStation( frequency );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
    station.setRadioTextPlus( rtClass, rtItem );
    q->saveStation( station );
    mUiEngine.playLogModel().addRadioTextPlus( rtClass, rtItem );
}

/*!
 * \reimp
 * Sets the PI code to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentPiCode( uint frequency, int piCode )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findCurrentStation( frequency );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
#ifdef SHOW_CALLSIGN_IN_ANY_REGION
    RadioRegion::Region region = RadioRegion::America;
#else
    RadioRegion::Region region =  mEngine->region();
#endif

    station.setPiCode( piCode, region );
    q->saveStation( station );
}
