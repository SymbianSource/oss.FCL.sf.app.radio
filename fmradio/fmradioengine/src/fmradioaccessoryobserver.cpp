/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  accessory observer
*
*/


#include <AccPolGenericID.h>

#include "fmradioaccessoryobserver.h"
#include "fmradioheadseteventobserver.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryObserver::CFMRadioAccessoryObserver() : 
        CActive( CActive::EPriorityStandard ),
        iObserver( NULL )
    {
	FTRACE( FPrint(_L("CFMRadioAccessoryObserver::CFMRadioAccessoryObserver()") ) );
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryObserver::ConstructL()
    {
	FTRACE( FPrint(_L("CFMRadioAccessoryObserver::ConstructL() -- ENTER") ) )
    User::LeaveIfError( iAccessoryServer.Connect() );
    
    // Creates a new sub-session within an existing session.
    User::LeaveIfError( iAccessoryModeSession.CreateSubSession( iAccessoryServer ) );
    User::LeaveIfError( iAccessoryModeSession.GetAccessoryMode( iAccPolAccessoryMode ) );

    iPrevAccMode = iAccPolAccessoryMode;
    
    CActiveScheduler::Add( this );      
    // Accessory mode is always listened
    iAccessoryModeSession.NotifyAccessoryModeChanged( iStatus, iAccPolAccessoryMode ); 
    SetActive();
	FTRACE( FPrint(_L("CFMRadioAccessoryObserver::ConstructL() -- EXIT") ) )
    }

    
// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryObserver* CFMRadioAccessoryObserver::NewL()
    {
    CFMRadioAccessoryObserver* self = new( ELeave ) CFMRadioAccessoryObserver;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryObserver::~CFMRadioAccessoryObserver()
    {
    Cancel();
	iAccessoryModeSession.CloseSubSession();
	iAccessoryServer.Close();
    }


// ---------------------------------------------------------------------------
// CFMRadioAccessoryObserver::IsHeadsetAccessoryConnected
// ---------------------------------------------------------------------------
//
TBool CFMRadioAccessoryObserver::IsHeadsetAccessoryConnected() const
	{
	TBool accessoryConnected;
	
#ifdef __WINS__
    accessoryConnected = ETrue;
#else
	switch ( iAccPolAccessoryMode.iAccessoryMode )
        {
        case EAccModeWiredHeadset:    // Falls through.
        case EAccModeLoopset:         // Falls through.
        case EAccModeHeadphones:
        case EAccModeMusicStand:
            {
            accessoryConnected = ETrue;
	        break;
            }
        default:
            {
			accessoryConnected = EFalse;
			break;
			}
        }
#endif   
	return accessoryConnected;
	}


// ---------------------------------------------------------------------------
// CFMRadioAccessoryObserver::SetObserver
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryObserver::SetObserver( MFMRadioHeadsetEventObserver* aObserver )
	{
	iObserver = aObserver;
	}

	
// ---------------------------------------------------------------------------
// From class CActive
// CFMRadioAccessoryObserver::RunL
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryObserver::RunL()
    {    
    TRequestStatus status = iStatus;
    if ( status == KErrNone )
        {
        // Accessory mode may change when combined connection status changes 
        // or when audio routing status changes. 
        if ( iObserver )
            {        		
	        if ( iPrevAccMode.iAccessoryMode == iAccPolAccessoryMode.iAccessoryMode &&
	        		iPrevAccMode.iAudioOutputStatus != iAccPolAccessoryMode.iAudioOutputStatus )
	        	{
	            // do nothing since we don't want another callback from same event	
	        	}
	        else
	        	{
				if ( IsHeadsetAccessoryConnected() )
					{
					iObserver->HeadsetAccessoryConnectedCallbackL();
					}
				else
					{
					iObserver->HeadsetAccessoryDisconnectedCallbackL();
					}
	        	}
	        iPrevAccMode = iAccPolAccessoryMode;
        	}
        }
    iAccessoryModeSession.NotifyAccessoryModeChanged( iStatus, iAccPolAccessoryMode ); 
    SetActive(); 
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// CFMRadioAccessoryObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryObserver::DoCancel()
    {
    iAccessoryModeSession.CancelNotifyAccessoryModeChanged();
    }
