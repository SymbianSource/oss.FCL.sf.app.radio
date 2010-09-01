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
* Description: Implementation for the FM Radio Action Handler Plugin.
*
*/

#include <e32cmn.h>
#include <e32property.h>
#include <e32std.h>
#include <w32std.h>
#include <ecom/implementationproxy.h>
#include <liwvariant.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <fmradiocommandlineparams.h>
#include <fmradiointernalpskeys.h>
#include <fmradiouids.h>

#include "fmradioactionhandler.h"
#include "fmradioactionhandler.hrh"
#include "fmradioactionhandlerdefs.h"

// ---------------------------------------------------------
// CFMRadioActionHandler::NewL
// ---------------------------------------------------------
//
CFMRadioActionHandler* CFMRadioActionHandler::NewL()
    {
    CFMRadioActionHandler* self = new ( ELeave ) CFMRadioActionHandler(); 
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::~CFMRadioActionHandler
// ---------------------------------------------------------
//
CFMRadioActionHandler::~CFMRadioActionHandler()
    {
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::CFMRadioActionHandler
// ---------------------------------------------------------
//
CFMRadioActionHandler::CFMRadioActionHandler()
    {
    }
    
// ---------------------------------------------------------
// CFMRadioActionHandler::ConstructL
// ---------------------------------------------------------
//
void CFMRadioActionHandler::ConstructL()
    {
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::ExecuteActionL
// ---------------------------------------------------------
//
TInt CFMRadioActionHandler::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt retValue( KErrNone );
    RBuf command;
    CleanupClosePushL( command );
    
    retValue = ExtractStringL( aMap, command, KFMRadioCommand);
    if ( retValue == KErrNone )
        {
        if ( !command.CompareF( KFMRadioCommandValueStepPrevious ) )
            {
            StepPrevious();
            }
        else if ( !command.CompareF( KFMRadioCommandValueStepNext ) )
            {
            StepNext();
            }
        else if ( !command.CompareF( KFMRadioCommandValueMute) )
            {
            Mute();
            }
        else if ( !command.CompareF( KFMRadioCommandValueUnmute ) )
            {
            Unmute();
            }
        else if ( !command.CompareF( KFMRadioCommandValueSeekUp ) )
            {
            SeekUp();
            }
        else if ( !command.CompareF( KFMRadioCommandValueSeekDown ) )
            {
            SeekDown();
            }
        else if ( !command.CompareF( KFMRadioCommandValueStartNowPlaying ) )
            {
            StartToNowPlayingL();
            }
        else
            {
            retValue = KErrNotSupported;
            }
        }
    CleanupStack::PopAndDestroy( &command );
    return retValue;
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::StepPrevious
// ---------------------------------------------------------
//
void CFMRadioActionHandler::StepPrevious()
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlStepToChannel, EFMRadioPSStepToChannelDown );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::StepNext
// ---------------------------------------------------------
//
void CFMRadioActionHandler::StepNext()
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlStepToChannel, EFMRadioPSStepToChannelUp );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::Mute
// ---------------------------------------------------------
//
void CFMRadioActionHandler::Mute()
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlSetRadioMuteState, EFMRadioPSMuteStateOn );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::Unmute
// ---------------------------------------------------------
//
void CFMRadioActionHandler::Unmute()
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlSetRadioMuteState, EFMRadioPSMuteStateOff );    
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::SeekUp
// ---------------------------------------------------------
//
void CFMRadioActionHandler::SeekUp()
    {
    //RProperty::Set( KFMRadioPSUid, KFMRadioPSControlSeek, EFMRadioPSSeekUp );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::SeekDown
// ---------------------------------------------------------
//
void CFMRadioActionHandler::SeekDown()
    {
    //RProperty::Set( KFMRadioPSUid, KFMRadioPSControlSeek, EFMRadioPSSeekDown );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::StartToNowPlayingL
// ---------------------------------------------------------
//
void CFMRadioActionHandler::StartToNowPlayingL()
    {
    RWsSession wsSession;
                
    User::LeaveIfError(wsSession.Connect() );
    CleanupClosePushL( wsSession );
    
    TApaTaskList taskList( wsSession );
    TUid appUid = TUid::Uid( KUidFMRadioApplication );
    TApaTask fmRadioTask = taskList.FindApp( appUid );
    
    const TInt KFMRadioViewIdLength = 3;
    RBuf8 params;
    params.CreateL( KFMRadioCommandActivateView().Length() + KFMRadioViewIdLength );
    params.Copy( KFMRadioCommandActivateView );
    params.Append( _L(" ") );
    params.AppendNum( KFMRadioMainViewId.iUid );
    params.CleanupClosePushL();
    
    if ( !fmRadioTask.Exists() )
        {
        RApaLsSession apaSession;
        CleanupClosePushL( apaSession );
                
        User::LeaveIfError( apaSession.Connect() );
        User::LeaveIfError( apaSession.GetAllApps() );
                
        TApaAppInfo appInfo;
        User::LeaveIfError( apaSession.GetAppInfo( appInfo, appUid ) );
                
        CApaCommandLine* startParams = CApaCommandLine::NewLC();
        startParams->SetExecutableNameL( appInfo.iFullName );
        startParams->SetCommandL( EApaCommandRun ); 
        startParams->SetTailEndL( params );
        User::LeaveIfError( apaSession.StartApp( *startParams ) );
        CleanupStack::PopAndDestroy( 2, &apaSession );
        }
    else
        {
        User::LeaveIfError( fmRadioTask.SendMessage( appUid, params ) );
        }
    CleanupStack::PopAndDestroy( 2 );
    }

// ---------------------------------------------------------
// CFMRadioActionHandler::ExtractStringLC
// ---------------------------------------------------------
//
TInt CFMRadioActionHandler::ExtractStringL( const CLiwMap* aMap, RBuf& aString, const TDesC8& aMapName )
    {
    TInt err( KErrNotFound );
    TLiwVariant variant;
    variant.PushL();
    TPtrC tempString( KNullDesC );    
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( tempString );
        aString.Close();
        aString.CreateL( tempString );
        err = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );    
    return err;
    }

/** Provides a key-value pair table, that is used to identify the correct construction function for the requested interface. */
const TImplementationProxy KFMRadioActionHandlerImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFMRadioActionHandlerImplementationUid, CFMRadioActionHandler::NewL )
    };

const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

// ---------------------------------------------------------------------------
// Returns the implementations provided by this ECOM plugin DLL.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KFMRadioActionHandlerImplementationTable ) / sizeof( TImplementationProxy );
    return KFMRadioActionHandlerImplementationTable;
    }
