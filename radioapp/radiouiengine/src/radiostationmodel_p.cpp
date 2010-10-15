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
#include <QTimer>

// User includes
#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiologger.h"
#include "radiopresetstorage.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radiostation.h"
#include "radiostation_p.h"
#include "radiohistorymodel.h"

// Constants
/**
 * Timeout period for checking if station is sending dynamic PS in milliseconds
 */
const int DYNAMIC_PS_CHECK_TIMEOUT = 10 * 1000; // 10 seconds

const int RADIOTEXT_CLEAR_TIMEOUT = 1 * 60 * 1000; // One minute
const int RTPLUS_CHECK_TIMEOUT = 700;

enum TimerId{ RtPlusCheck = 999 };

const QLatin1String HTML_TAG_START( "<font color='cyan'><u>" );
const QLatin1String HTML_TAG_END( "</u></font>" );

/*!
 *
 */
RadioStationModelPrivate::RadioStationModelPrivate( RadioStationModel* model,
                                                    RadioUiEnginePrivate& uiEngine ) :
    q_ptr( model ),
    mUiEngine( uiEngine ),
    mCurrentStation( &mManualStation ),
    mDynamicPsTimer( new QTimer() )
{
    mManualStation.setType( RadioStation::ManualStation );
    Radio::connect( mDynamicPsTimer.data(), SIGNAL(timeout()),
                    q_ptr,                  SLOT(dynamicPsCheckEnded()) );
    mDynamicPsTimer->setInterval( DYNAMIC_PS_CHECK_TIMEOUT );
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
    RadioStation* oldStation = mCurrentStation;
    if ( mStations.contains( frequency ) ) {
        // We have to be careful to check that key exists before using operator[]
        // with QMap since it will insert a default-constructed value if it doesn't exist yet.
        mCurrentStation = &mStations[ frequency ];
    } else {
        mManualStation.reset();
        mManualStation.setType( RadioStation::ManualStation );
        mManualStation.setFrequency( frequency );
        mCurrentStation = &mManualStation;
    }

    Q_Q( RadioStationModel );
    if ( oldStation && oldStation->isValid() ) {
        q->emitDataChanged( *oldStation );
        q->emitDataChanged( *mCurrentStation );
    }
}

/*!
 * \reimp
 * Sets the genre to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentGenre( uint frequency, int genre )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findStation( frequency, FindCriteria::IncludeManualStation );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
    station.setGenre( genre );
    q->saveStation( station );
}

/*!
 * \reimp
 *
 */
void RadioStationModelPrivate::tunedToFrequency( uint frequency, int reason )
{
    if ( !mRadioTextHolder.isEmpty() ) {
        mRadioTextHolder.clear();
        Q_Q( RadioStationModel );
        mUiEngine.api().cancelTimer( RtPlusCheck, q );
    }

    setCurrentStation( frequency );
    startDynamicPsCheck();
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
 * Sets the PS name to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentPsName( uint frequency, const QString& name )
{
    Q_Q( RadioStationModel );
    LOG_FORMAT( "RadioStationModelPrivate::setCurrentPsName: %s", GETSTRING( name ) );
    RadioStation station = q->findStation( frequency, FindCriteria::IncludeManualStation );
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
                     && mWrapper->region() != RadioRegion::America )
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
    RadioStation station = q->findStation( frequency, FindCriteria::IncludeManualStation );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }

    mUiEngine.api().historyModel().clearRadioTextPlus();

    mRadioTextHolder = radioText;
    mRadioTextPlusItems.clear();

    if ( !radioText.isEmpty() ) {
        mUiEngine.api().startTimer( RTPLUS_CHECK_TIMEOUT, RtPlusCheck,
                                    q, SLOT(rtPlusCheckEnd()) );
    } else {
        station.setRadioText( "" );
        q->saveStation( station );
    }
}

/*!
 * \reimp
 * Sets the radio text plus to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentRadioTextPlus( uint frequency, int rtClass, const QString& rtItem )
{
    LEVEL2( LOG_METHOD );

    Q_Q( RadioStationModel );
    RadioStation station = q->findStation( frequency, FindCriteria::IncludeManualStation );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }

    if ( mRadioTextPlusItems.isEmpty() ) { // First RT tag has arrived

        const RtPlus::Tag tag = recognizeRtPlusTag( rtClass );
        mRadioTextPlusItems.insert( tag, rtItem );

    } else { // Second RT tag has arrived

        const RtPlus::Tag tag = recognizeRtPlusTag( rtClass );
        mRadioTextPlusItems.insert( tag, rtItem );

        handleRadioTextPlus( mRadioTextPlusItems, station );
        mRadioTextPlusItems.clear();

    }
}

/*!
 * \reimp
 * Sets the PI code to the currently tuned station
 */
void RadioStationModelPrivate::setCurrentPiCode( uint frequency, int piCode )
{
    Q_Q( RadioStationModel );
    RadioStation station = q->findStation( frequency, FindCriteria::IncludeManualStation );
    if ( !station.isValid() ) {
        LOG( "Unable to find current station. Ignoring RDS" );
        return;
    }
#ifdef SHOW_CALLSIGN_IN_ANY_REGION
    RadioRegion::Region region = RadioRegion::America;
#else
    RadioRegion::Region region =  mWrapper->region();
#endif

    station.setPiCode( piCode, region );
    q->saveStation( station );
}

/*!
 *
 */
void RadioStationModelPrivate::doSaveStation( RadioStation& station, bool persistentSave )
{
    mStations.insert( station.frequency(), station );

    if ( persistentSave ) {
        const bool success = mPresetStorage->savePreset( *station.data_ptr() );
        RADIO_ASSERT( success, "RadioStationModelPrivate::saveStation", "Failed to add station" );
        Q_UNUSED( success );
    }
}

/*!
 *
 */
void RadioStationModelPrivate::doRemoveStation( const RadioStation& station, bool signalFrequency )
{
    Q_Q( RadioStationModel );
    const uint frequency = station.frequency();
    if ( mStations.contains( frequency ) ) {

        // If we are removing the current station, copy its data to the current station pointer
        // to keep all of the received RDS data still available. Name and favorite setting have  
        // to be cleared. Station data will be discarded when the user tunes to another frequency, 
        // but they are available if the user decides to add it back.
        if ( mCurrentStation->frequency() == frequency ) {
            *mCurrentStation = station;
            mCurrentStation->setFavorite( false );
            mCurrentStation->setUserDefinedName( "" );
            q->emitChangeSignals( *mCurrentStation, mCurrentStation->changeFlags() );
        }

        const int row = q->indexFromFrequency( station.frequency() );
        q->beginRemoveRows( QModelIndex(), row, row );
        
        mPresetStorage->deletePreset( station.presetIndex() );
        mStations.remove( frequency );

        mCurrentStation = NULL;
        setCurrentStation( mWrapper->currentFrequency() );
        
        q->endRemoveRows();
        if( signalFrequency ) {
            emit q->stationsRemoved( QList<uint>() << frequency );
        }
    }
}

/*!
 *
 */
QList<RadioStation> RadioStationModelPrivate::favorites() const
{
    QList<RadioStation> favoriteList;
    foreach( const RadioStation& tempStation, mStations ) {
        if ( tempStation.isFavorite() ) {
            favoriteList.append( tempStation );
        }
    }
    return favoriteList;
}

/*!
 *
 */
QList<RadioStation> RadioStationModelPrivate::locals() const
{
    QList<RadioStation> localList;
    foreach( const RadioStation& tempStation, mStations ) {
        if ( !tempStation.isFavorite() ) {
            localList.append( tempStation );
        }
    }
    return localList;
}

/*!
 *
 */
void RadioStationModelPrivate::startRadioTextClearTimer()
{
    Q_Q( RadioStationModel );
    mUiEngine.api().startTimer( RADIOTEXT_CLEAR_TIMEOUT,
                                static_cast<int>( mCurrentStation->frequency() ),
                                q,
                                SLOT(clearRadiotext(int)) );
}

/*!
 *
 */
RtPlus::Tag RadioStationModelPrivate::recognizeRtPlusTag( int rtClass )
{
    switch ( rtClass ) {

        case RtPlus::Dummy:
            return RtPlus::Dummy;

        case RtPlus::Title:
            return RtPlus::Title;

        case RtPlus::Artist:
            return RtPlus::Artist;

        case RtPlus::Band:
            return RtPlus::Band;

        case RtPlus::Homepage:
            return RtPlus::Homepage;

        default:
            break;
    }

    return RtPlus::Unknown;
}

/*!
 *
 */
void RadioStationModelPrivate::handleRadioTextPlus( const RtPlusMap& rtItems, RadioStation& station )
{
    Q_Q( RadioStationModel );
    if ( rtItems.contains( RtPlus::Homepage ) ) {

        station.setUrl( rtItems.value( RtPlus::Homepage ) );
        q->saveStation( station );

    } else if ( rtItems.contains( RtPlus::Artist ) || rtItems.contains( RtPlus::Title ) ) {

        mUiEngine.api().cancelTimer( RtPlusCheck, q );

        QList<RtPlus::Tag> tags;
        tags << RtPlus::Artist << RtPlus::Title;
        foreach( RtPlus::Tag tag, tags ) {
            QString item = rtItems.value( tag );
            if ( !item.isEmpty() ) {
                const QString replacement = HTML_TAG_START + item + HTML_TAG_END;
                mRadioTextHolder.replace( item, replacement );
            }
        }
        station.setRadioText( mRadioTextHolder );
        mRadioTextHolder.clear();
        q->saveStation( station );
        startRadioTextClearTimer();
        mUiEngine.api().historyModel().addItem( rtItems.value( RtPlus::Artist ), rtItems.value( RtPlus::Title ), station );

    }
}
