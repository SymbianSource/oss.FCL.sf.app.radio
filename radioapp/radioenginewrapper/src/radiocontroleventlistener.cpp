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

#include "radiocontroleventlistener.h"
#include "radioenginewrapper_p.h"
#include "radioenginewrapperobserver.h"
#include "cradioenginehandler.h"
#include "radiologger.h"
#include "cradioremcontarget.h"
/*!
 *
 */
RadioControlEventListener::RadioControlEventListener( RadioEngineWrapperPrivate& engine ) :
    mEngine( engine )
{
}

/*!
 *
 */
RadioControlEventListener::~RadioControlEventListener()
{
}

/*!
 *
 */
void RadioControlEventListener::init()
{
    TRAPD( err, mRemCon.reset( CRadioRemConTarget::NewL() ) );
    LOG_ASSERT( !err, LOG_FORMAT( "RadioControlEventListener::init Failed with err, %d", err ) );

    if ( !err ) {
        mRemCon->SetControlEventObserver( this );
    }
}

/*!
 *
 */
void RadioControlEventListener::StepToChannelL( RadioEngine::TRadioTuneDirection DEBUGVAR( aDirection ) )
{
    LOG_FORMAT( "RadioControlEventListener::StepToChannelL: direction: %d", aDirection );
}

/*!
 *
 */
void RadioControlEventListener::SetChannelL( TInt DEBUGVAR( aChannelId ) )
{
    LOG_FORMAT( "RadioControlEventListener::SetChannelL: Channel: %d", aChannelId );
}

/*!
 *
 */
void RadioControlEventListener::SeekL( RadioEngine::TRadioTuneDirection aDirection )
{
    LOG_FORMAT( "RadioControlEventListener::SeekL: Direction: %d", aDirection );
    mEngine.startSeeking( aDirection == RadioEngine::ERadioDown ? Seeking::Down : Seeking::Up );
}

/*!
 *
 */
void RadioControlEventListener::StepToFrequencyL( RadioEngine::TRadioTuneDirection DEBUGVAR( aDirection ) )
{
    LOG_FORMAT( "RadioControlEventListener::StepToFrequencyL: Direction: %d", aDirection );
}

/*!
 *
 */
void RadioControlEventListener::SetFrequencyL( TUint32 DEBUGVAR( aFreq ) )
{
    LOG_FORMAT( "RadioControlEventListener::SetFrequencyL: Frequency: %d", aFreq );
}

/*!
 *
 */
void RadioControlEventListener::AdjustVolumeL( RadioEngine::TRadioVolumeSetDirection DEBUGVAR( aDirection ) )
{
    LOG_FORMAT( "RadioControlEventListener::AdjustVolumeL: Direction: %d", aDirection );
}

/*!
 *
 */
void RadioControlEventListener::MuteL( TBool aMute )
{
    LOG_FORMAT( "RadioControlEventListener::MuteL: Mute: %d", aMute );
    mEngine.RadioEnginehandler().SetMuted( aMute );
}

/*!
 *
 */
void RadioControlEventListener::PlayL( TBool aDownPressed )
{
    if( aDownPressed )
        {
        mEngine.RadioEnginehandler().SetMuted( false );
        }
}

/*!
 *
 */
void RadioControlEventListener::PauseL( TBool aDownPressed )
{
    if( aDownPressed )
        {
        mEngine.RadioEnginehandler().SetMuted( true );
        }
}

/*!
 *
 */
void RadioControlEventListener::PausePlayL( TBool aDownPressed )
{
    if( aDownPressed )
        {
        const TBool muted = !mEngine.RadioEnginehandler().IsMuted();
        mEngine.RadioEnginehandler().SetMuted( muted );
        }
}

/*!
 *
 */
void RadioControlEventListener::StopL( TBool aDownPressed )
{
    if( aDownPressed )
        {
        mEngine.RadioEnginehandler().SetMuted( ETrue );
        }
}

/*!
 *
 */
void RadioControlEventListener::ForwardL( TBool aDownPressed )
{
    if( aDownPressed ) {
        RUN_NOTIFY_LOOP( mEngine.observers(), skipNext() );
    }
}

/*!
 *
 */
void RadioControlEventListener::FastForwardL( TBool aDownPressed )
{
    if( aDownPressed )
    {
        mEngine.startSeeking( Seeking::Up );
    }
}

/*!
 *
 */
void RadioControlEventListener::BackwardL( TBool aDownPressed )
{
    if( aDownPressed ) {
        RUN_NOTIFY_LOOP( mEngine.observers(), skipPrevious() );
    }
}

/*!
 *
 */
void RadioControlEventListener::RewindL( TBool aDownPressed )
{
    if( aDownPressed ) {
        mEngine.startSeeking( Seeking::Down );
    }
}

/*!
 *
 */
void RadioControlEventListener::ChannelUpL( TBool DEBUGVAR( aDownPressed ) )
{
    LOG_FORMAT( "RadioControlEventListener::ChannelUpL: Down: %d", aDownPressed );
}

/*!
 *
 */
void RadioControlEventListener::ChannelDownL( TBool DEBUGVAR( aDownPressed ) )
{
    LOG_FORMAT( "RadioControlEventListener::ChannelDownL: Down: %d", aDownPressed );
}

/*!
 *
 */
void RadioControlEventListener::VolumeUpL( TBool aDownPressed )
{
    if ( aDownPressed ) {
        mEngine.RadioEnginehandler().IncreaseVolume();
    }
}

/*!
 *
 */
void RadioControlEventListener::VolumeDownL( TBool aDownPressed )
{
    if ( aDownPressed ) {
        mEngine.RadioEnginehandler().DecreaseVolume();
    }
}

/*!
 *
 */
void RadioControlEventListener::ChangeViewL( TUid DEBUGVAR( aViewId ), TBool DEBUGVAR( aForceChange ) )
{
    LOG_FORMAT( "RadioControlEventListener::ChangeViewL: View: %d, Force: %d", aViewId, aForceChange );
}

/*!
 *
 */
void RadioControlEventListener::SetAudioRouteL( RadioEngine::TRadioAudioRoute DEBUGVAR( aRoute ) )
{
    LOG_FORMAT( "RadioControlEventListener::SetAudioOutputDestinationL: Destinatio: %d", aRoute );
}

/*!
 *
 */
void RadioControlEventListener::AnswerEndCallL()
{
    RUN_NOTIFY_LOOP( mEngine.observers(), skipNext() );
}
