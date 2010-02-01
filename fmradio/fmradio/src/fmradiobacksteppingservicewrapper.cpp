/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for the Back Stepping Service wrapper
*
*/

#include <liwcommon.h>
#include <liwservicehandler.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "fmradiobacksteppingservicewrapper.h"

_LIT8( KFMRadioBackSteppingPrefix, "FMRADIOBS" );

// BS Service implementation UID
//const TInt KBSServiceImplUid( 0x2000F840 );

// BS Service and Interface ID
_LIT8( KFMRadioBSServiceID, "Service.BackStepping" );
_LIT8( KFMRadioBSInterface, "IBackStepping" );

// BS commands
_LIT8( KFMRadioBSCmdInitialize, "Initialize" );
_LIT8( KFMRadioBSCmdForwardActivationEvent, "ForwardActivationEvent" );
_LIT8( KFMRadioBSCmdHandleBackCommand, "HandleBackCommand" );

// BS in param names
_LIT8( KFMRadioBSInParamAppUid, "AppUid" );
_LIT8( KFMRadioBSInParamState, "State" );
_LIT8( KFMRadioBSInParamEnter, "Enter" );

// BS out param names
_LIT8( KFMRadioBSOutParamStatusInfo, "StatusInfo" );

// ======== MEMBER FUNCTIONS =================================================

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::NewL
// ---------------------------------------------------------------------------
//
CFMRadioBackSteppingServiceWrapper* CFMRadioBackSteppingServiceWrapper::NewL( TUid aUid )
    {
    CFMRadioBackSteppingServiceWrapper* self = new( ELeave ) CFMRadioBackSteppingServiceWrapper;
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::CFMRadioBackSteppingServiceWrapper
// ---------------------------------------------------------------------------
//
CFMRadioBackSteppingServiceWrapper::CFMRadioBackSteppingServiceWrapper()
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioBackSteppingServiceWrapper::ConstructL( TUid aUid )
    {
    // Create Service Handler and keep as long as access to BS Service needed.
    iServiceHandler = CLiwServiceHandler::NewL();
    // For convenience keep pointers to Service Handler param lists.
    iInParamList = &iServiceHandler->InParamListL();
    iOutParamList = &iServiceHandler->OutParamListL();

    // Create AIW criteria.
    RCriteriaArray criteriaArray;
    CleanupResetAndDestroyPushL( criteriaArray );
    CLiwCriteriaItem* criterion = CLiwCriteriaItem::NewLC(
        KLiwCmdAsStr,
        KFMRadioBSInterface,
        KFMRadioBSServiceID );
    criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    criteriaArray.AppendL( criterion );
    CleanupStack::Pop( criterion );

    // Attach AIW criteria.
    iServiceHandler->AttachL( criteriaArray );
    // Get BS Service interface.
    iServiceHandler->ExecuteServiceCmdL( *criterion, *iInParamList, *iOutParamList );
    CleanupStack::PopAndDestroy( &criteriaArray );

    // Check if BS interface can be found from output params.
    TInt pos( 0 );
    iOutParamList->FindFirst( pos, KFMRadioBSInterface );
    if( KErrNotFound != pos )
        {
        // Extract BS interface from output params.
        iBsInterface = ( *iOutParamList )[pos].Value().AsInterface();   
        }

    // Initialize BS Service.
    InitializeL( aUid );
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::~CFMRadioBackSteppingServiceWrapper
// ---------------------------------------------------------------------------
//
CFMRadioBackSteppingServiceWrapper::~CFMRadioBackSteppingServiceWrapper()
    {
    if ( iBsInterface )
        {
        iBsInterface->Close();
        }
    if ( iServiceHandler )
        {
        delete iServiceHandler;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::HandleViewActivationEventL
// ---------------------------------------------------------------------------
//
void CFMRadioBackSteppingServiceWrapper::HandleViewActivationEventL( const TVwsViewId aViewId,
                                                                     TBool aEnter )
    {
    if ( iBsInterface )
        {
        // Make Service Handler param lists.
        iInParamList->Reset();
        iOutParamList->Reset();
        
        RBuf8 buffer;
        CleanupClosePushL( buffer );
        buffer.CreateL( KFMRadioBackSteppingPrefix().Length() + KDefaultRealWidth );
        buffer.Copy( KFMRadioBackSteppingPrefix );
        // Convert 32-bit signed integer (TInt32) view uid to TInt.
        TInt64 viewUid = static_cast< TInt64 >( aViewId.iViewUid.iUid );
        buffer.AppendNum( viewUid );
            
        TLiwGenericParam state( KFMRadioBSInParamState, TLiwVariant( buffer ) );
        iInParamList->AppendL( state );
        TLiwGenericParam enter( KFMRadioBSInParamEnter, TLiwVariant( aEnter ) );
        iInParamList->AppendL( enter );

        // Execute view activation event.
        iBsInterface->ExecuteCmdL( KFMRadioBSCmdForwardActivationEvent, *iInParamList, *iOutParamList );
        CleanupStack::PopAndDestroy();

        // Check if BS Service consumed the event.
        HandleResultL();

        iInParamList->Reset();
        iOutParamList->Reset();
        }
    }


// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::HandleBackCommandL
// ---------------------------------------------------------------------------
//
TBool CFMRadioBackSteppingServiceWrapper::HandleBackCommandL( const TVwsViewId aViewId )
    {
    TBool retVal = EFalse;
    if ( iBsInterface )
        {
        // Make Service Handler param lists.
        iInParamList->Reset();
    	iOutParamList->Reset();
    	
        // Create message for back stepping wrapper to handle back command event.
        RBuf8 buffer;
        CleanupClosePushL( buffer );
        buffer.CreateL( KFMRadioBackSteppingPrefix().Length() + KDefaultRealWidth );
        buffer.Copy( KFMRadioBackSteppingPrefix );
        // Convert 32-bit signed integer (TInt32) view uid to TInt64.
        TInt64 viewUid = static_cast< TInt64 >( aViewId.iViewUid.iUid );
        buffer.AppendNum( viewUid );
        
    	TLiwGenericParam state( KFMRadioBSInParamState, TLiwVariant( buffer ) );
    	iInParamList->AppendL( state );
    
        // Execute back event.
    	iBsInterface->ExecuteCmdL( KFMRadioBSCmdHandleBackCommand, *iInParamList, *iOutParamList );
        CleanupStack::PopAndDestroy();
    
        // Check if BS Service consumes the event.
        retVal = HandleResultL();
    
    	iInParamList->Reset();
    	iOutParamList->Reset();
        }
    
	return retVal;
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::InitializeL
// ---------------------------------------------------------------------------
//
void CFMRadioBackSteppingServiceWrapper::InitializeL( const TUid aUid )
    {
    if ( iBsInterface )
        {
        // Initialize BS Service.
        iInParamList->Reset();
    	iOutParamList->Reset();
    
    	TLiwGenericParam appUid( KFMRadioBSInParamAppUid, TLiwVariant( aUid.iUid ) );
    	iInParamList->AppendL( appUid );
    	
    	iBsInterface->ExecuteCmdL( KFMRadioBSCmdInitialize, *iInParamList, *iOutParamList );
    
        if ( !HandleResultL() )
            {
            // Initialize command was not consumed.
            User::Leave( KErrArgument );
            }
    
    	iInParamList->Reset();
    	iOutParamList->Reset();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::HandleResultL
// ---------------------------------------------------------------------------
//
TInt CFMRadioBackSteppingServiceWrapper::HandleResultL()
    {
    TBool retVal( EFalse );
    // Check if error can be found from outparams.
    TInt posErr( 0 );
	iOutParamList->FindFirst( posErr, LIW::EGenericParamError );
	if( KErrNotFound != posErr )
		{
		// Error code found - extract and handle
		TInt errorCode( KErrNone );
		( *iOutParamList )[posErr].Value().Get( errorCode );
		User::LeaveIfError( errorCode );
		}
    // No errors found. Check if status info can be found from outparams.
    TInt posStat( 0 );
    iOutParamList->FindFirst( posStat, KFMRadioBSOutParamStatusInfo );
    if( KErrNotFound != posStat )
		{
		// Status info present - extract.
		( *iOutParamList )[posStat].Value().Get( retVal );
		}
	else
	    {
	    // No status info found.
	    User::Leave( KErrNotFound );
	    }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CFMRadioBackSteppingServiceWrapper::HandleViewActivation
// ---------------------------------------------------------------------------
//
void CFMRadioBackSteppingServiceWrapper::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
                                                               const TVwsViewId& /*aViewIdToBeDeactivated*/ )
    {
    TRAP_IGNORE( HandleViewActivationEventL( aNewlyActivatedViewId, ETrue ) )
    }
