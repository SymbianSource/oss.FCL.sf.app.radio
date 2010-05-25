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
* Description:  Observer of accessory connection.
*
*/
#include "fmradiovariant.hrh" // include first for variation
#include <AccessoryServer.h>
#include <AccessoryConnection.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDArray.h>
#include <AccPolGenericIdDefinitions.h>

#include "fmradioaccessoryconnection.h"
#include "fmradioheadseteventobserver.h"
#include "debug.h"

#ifdef __FMRADIO_ADVANCED_AUTO_RESUME // same flag can be used to determine correct mask
#include <internal/accpolpropgenericid.h>
const TUint KPhysicalConnectionBitmask = KPCNokiaAV | KPCWired;
#else 
const TUint KPhysicalConnectionBitmask = KPCWired;
#endif // __FMRADIO_ADVANCED_AUTO_RESUME

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::CFMRadioAccessoryConnection
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryConnection::CFMRadioAccessoryConnection() :
    CActive(EPriorityStandard), iObserver ( NULL ),
        iWiredHeadsetConnected ( EFalse ), iAccessoryCount( 0 )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::NewL
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryConnection* CFMRadioAccessoryConnection::NewL()
    {
    CFMRadioAccessoryConnection* self = new (ELeave) CFMRadioAccessoryConnection();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryConnection::ConstructL()
    {
    FTRACE( FPrint(_L("CFMRadioAccessoryConnection::ConstructL() -- ENTER") ) )
    
    // Creates a new session and sub-session.
    User::LeaveIfError( iAccessoryServer.Connect() );
    User::LeaveIfError( iAccessoryConnection.CreateSubSession( iAccessoryServer ) );
    User::LeaveIfError( iAccessoryConnection.GetAccessoryConnectionStatus( iAccessoryArray ) );
    
    iWiredHeadsetConnected = WiredHeadsetInArrayL();
    iAccessoryCount = iAccessoryArray.Count();
    CActiveScheduler::Add(this); // Add to scheduler
    
    iAccessoryConnection.NotifyAccessoryConnectionStatusChanged( iStatus, iAccessoryArray );
    SetActive(); // Tell scheduler a request is active
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::~CFMRadioAccessoryConnection
// ---------------------------------------------------------------------------
//
CFMRadioAccessoryConnection::~CFMRadioAccessoryConnection()
    {
    Cancel(); // Cancel any request, if outstanding
    iAccessoryConnection.CloseSubSession();
    iAccessoryServer.Close();
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::DoCancel
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryConnection::DoCancel()
    {
    iAccessoryConnection.CancelNotifyAccessoryConnectionStatusChanged();
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::WiredHeadsetInArrayL
// ---------------------------------------------------------------------------
//
TBool CFMRadioAccessoryConnection::WiredHeadsetInArrayL()
    {
    FTRACE(FPrint(_L("CFMRadioAccessoryConnection::WiredHeadsetInArray()-- ENTER")));
    TAccPolGenericID genId;
    TUint32 devCaps;
    TUint32 phyCaps;
    
    for ( TInt i = 0; i < iAccessoryArray.Count(); i++ )
        {
        genId = iAccessoryArray.GetGenericIDL( i );
        devCaps = genId.DeviceTypeCaps();
        phyCaps = genId.PhysicalConnectionCaps();
        FTRACE(FPrint(_L("CFMRadioAccessoryConnection::devCaps-- %u"), devCaps ));
        FTRACE(FPrint(_L("CFMRadioAccessoryConnection::phyCaps()-- %u"), phyCaps ));
        
        //we're explicitly interested about wired headset type, without extra caps masked in
        if ( devCaps == KDTHeadset && phyCaps == KPhysicalConnectionBitmask )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::WiredHeadsetConnected
// ---------------------------------------------------------------------------
//
TBool CFMRadioAccessoryConnection::WiredHeadsetConnected()
    {
#ifdef __WINS__
    return ETrue;
#else
    return iWiredHeadsetConnected;
#endif
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::WiredHeadsetFirstInArray
// ---------------------------------------------------------------------------
//
TBool CFMRadioAccessoryConnection::WiredHeadsetFirstInArrayL()
    {
    FTRACE(FPrint(_L("CFMRadioAccessoryConnection::WiredHeadsetFirstInArrayL()-- ENTER")));
    if ( iAccessoryArray.Count() > 0 )
        {
        TAccPolGenericID genId = iAccessoryArray.GetGenericIDL( 0 );
        
        TUint32 devCaps = genId.DeviceTypeCaps();
        TUint32 phyCaps = genId.PhysicalConnectionCaps();
        
        FTRACE(FPrint(_L("CFMRadioAccessoryConnection::devCaps-- %u"), devCaps ));
        FTRACE(FPrint(_L("CFMRadioAccessoryConnection::phyCaps-- %u"), phyCaps ));
        
        //we're explicitly interested about wired headset type, without extra caps masked in
        if ( devCaps == KDTHeadset && phyCaps == KPhysicalConnectionBitmask )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::RunL
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryConnection::RunL()
    {
    FTRACE(FPrint(_L("CFMRadioAccessoryConnection::RunL()-- ENTER")));
    if ( iStatus == KErrNone )
        {
        if ( iAccessoryCount < iAccessoryArray.Count() )
            { //accessory has been added
            if ( WiredHeadsetFirstInArrayL() ) //and it is a wired headset
                {
                iWiredHeadsetConnected = ETrue;
                if ( iObserver )
                    {
                    iObserver->HeadsetAccessoryConnectedCallbackL();
                    }
                }
            }
        else
            { //accessory has been removed
            if ( !WiredHeadsetInArrayL() ) // and it is a wired headset
                {
                iWiredHeadsetConnected = EFalse;
                if ( iObserver )
                    {
                    iObserver->HeadsetAccessoryDisconnectedCallbackL();
                    }
                }
            }
        iAccessoryCount = iAccessoryArray.Count();
        }
    //reassign the notification request
    iAccessoryConnection.NotifyAccessoryConnectionStatusChanged( iStatus, iAccessoryArray );
    SetActive(); // Tell scheduler a request is active
    FTRACE(FPrint(_L("CFMRadioAccessoryConnection::RunL()-- EXIT")));
    }

// ---------------------------------------------------------------------------
// CFMRadioAccessoryConnection::SetObserver
// ---------------------------------------------------------------------------
//
void CFMRadioAccessoryConnection::SetObserver( MFMRadioHeadsetEventObserver* aObserver )
    {
    iObserver = aObserver;
    }

//end of file
