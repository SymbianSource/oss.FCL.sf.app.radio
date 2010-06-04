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

// User includes
#include "cradioaccessoryobserver.h"
#include "mradioheadseteventobserver.h"
#include "cradioenginelogger.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::CRadioAccessoryObserver()
    : CActive( CActive::EPriorityStandard )
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::ConstructL()
    {
    LOG_METHOD_AUTO;
    User::LeaveIfError( iAccessoryServer.Connect() );

    // Creates a new sub-session within an existing session.
    User::LeaveIfError( iAccessoryConn.CreateSubSession( iAccessoryServer ) );
    User::LeaveIfError( iAccessoryConn.GetAccessoryConnectionStatus( iAccPolGenIdArr ) );

    TInt nroChangedAccessories( 0 );
    UpdateCurrentAccessoriesL( nroChangedAccessories );
    LOG_FORMAT( "CRadioAccessoryObserver::ConstructL() -- Found %d wired/BT accessories ( %d accessories in total ).",
              nroChangedAccessories, iAccPolGenIdArr.Count() );
    CActiveScheduler::Add( this );
    // Accessory is always listened
    iAccessoryConn.NotifyAccessoryConnectionStatusChanged( iStatus, iAccPolGenIdArr );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver* CRadioAccessoryObserver::NewL()
    {
    CRadioAccessoryObserver* self = new( ELeave ) CRadioAccessoryObserver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::~CRadioAccessoryObserver()
    {
    Cancel();
    iAccessoryConn.CloseSubSession();
    iAccessoryServer.Close();
    iSavedAccessories.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::FindAccessoryL( TBool aAcceptOnlyHeadset, TAccAudioOutPutType& aOutputType ) const
    {
#ifdef __WINS__
    TBool accessoryFound = ETrue;
    aAcceptOnlyHeadset = ETrue;
    aOutputType = EAccAudioOutPutTypePrivate;
#else
    TBool accessoryFound = EFalse;

    LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - Connected accessories count = %d", iAccPolGenIdArr.Count());
    for ( TInt i = 0; i < iAccPolGenIdArr.Count() && !accessoryFound; i++ )
        {
        TAccPolGenericID accPolGenId = iAccPolGenIdArr.GetGenericIDL( i );

        //Check if physical connection is of acceptable type
        TUint32 physicalConnectionCaps = accPolGenId.PhysicalConnectionCaps();
        if ( physicalConnectionCaps & KPCWired || physicalConnectionCaps & KPCBluetooth )
            {
            LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - is wired or BT ( physicalConnectionCaps = %d )", physicalConnectionCaps );

            if ( !aAcceptOnlyHeadset )
                {
                accessoryFound = ETrue;
                }
            //Get device type ( check if is HeadSet )
            TUint32 deviceType = accPolGenId.DeviceTypeCaps();
            if ( deviceType & KDTHeadset )
                {
                LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - is HeadSet( deviceType = %d )", deviceType );
                if ( !IsLineoutConnectedL( accPolGenId ) )
                    {
                    LOG( "CRadioAccessoryObserver::FindAccessoryL - Is a normal headSet" );
                    accessoryFound = ETrue;
                    }
                }
            else if ( deviceType & KDTLoopset )
                {
                LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - is Loopset( deviceType = %d )", deviceType );
                accessoryFound = ETrue;
                }
            else
                {
                LOG_FORMAT( "CRadioAccessoryObserver::FindWiredAccessoryL - is NOT HeadSet or Loopset( deviceType = %d )", deviceType );
                }

            if ( accessoryFound )
                {
                // get the public/private status
                TAccPolNameRecord nameRecord;
                nameRecord.SetNameL( KAccAudioOutputType );

                TAccValueTypeTInt audioOutputTypeValue;
                iAccessoryConn.GetValueL( accPolGenId, nameRecord, audioOutputTypeValue );

                aOutputType = static_cast<TAccAudioOutPutType>( audioOutputTypeValue.iValue );
                LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - AudioOutputType = %d", aOutputType );
                }
            }
        else
            {
            LOG_FORMAT( "CRadioAccessoryObserver::FindAccessoryL - is NOT wired or BT ( physicalConnectionCaps = %d )", physicalConnectionCaps );
            }
        }
#endif
    return accessoryFound;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsHeadsetConnectedL() const
    {
    TAccAudioOutPutType audioOutputType( EAccAudioOutPutTypePrivate );
    return FindAccessoryL( ETrue , audioOutputType );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsLineoutConnectedL( TAccPolGenericID& aAccPolGenId ) const
    {
    TBool isLineOut( EFalse );

    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( nameArray );
    iAccessoryConn.GetSubblockNameArrayL( aAccPolGenId, *nameArray );

    TBool isAudioOutValue = nameArray->HasName( KAccAudioOut );
    CleanupStack::PopAndDestroy( nameArray );
    if ( isAudioOutValue )
        {
        LOG_FORMAT( "CRadioAccessoryObserver::IsLineoutConnectedL - isAudioOutValue = ( %d )", isAudioOutValue );

        TAccPolNameRecord nameRecord;
        nameRecord.SetNameL( KAccAudioOut );

        //Check if connected accessory is LineOut
        TAccValueTypeTInt audioOutValue;
        iAccessoryConn.GetValueL( aAccPolGenId, nameRecord, audioOutValue );

        LOG( "CRadioAccessoryObserver::IsLineoutConnectedL - GetValueL() - OK" );
        if ( TAccAudioOutLineout == audioOutValue.iValue )
            {
            //is lineout
            LOG_FORMAT( "CRadioAccessoryObserver::IsLineoutConnectedL - Accessory is LineOut( audioOutValue = %d )", audioOutValue.iValue );
            isLineOut = ETrue;
            }
        }
    return isLineOut;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::UpdateCurrentAccessoriesL( TInt& aNroChangedAccessories )
    {
    // first check, the removed accessories.
    TInt removedAccessories = CountRemovedAccessoriesL();
    LOG_FORMAT( "CRadioAccessoryObserver::UpdateCurrentAccessoriesL() -- %d accessories removed.", removedAccessories );
    aNroChangedAccessories += removedAccessories;
    TInt addedAccessories = CountAddedAccessoriesL();
    LOG_FORMAT( "CRadioAccessoryObserver::UpdateCurrentAccessoriesL() -- %d accessories added.", addedAccessories );
    aNroChangedAccessories += addedAccessories;

    iSavedAccessories.Reset();

    for ( TInt i = 0; i < iAccPolGenIdArr.Count(); i++ )
        {
        iSavedAccessories.AppendL( iAccPolGenIdArr.GetGenericIDL( i ) );
        LOG_FORMAT( "CRadioAccessoryObserver::UpdateCurrentAccessoriesL() -- accessory in slot %d: %d",
                  i, iAccPolGenIdArr.GetGenericIDL( i ).UniqueID() );
        }
    LOG_FORMAT( "CRadioAccessoryObserver::UpdateCurrentAccessoriesL() -- %d accessories in total.", iSavedAccessories.Count() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioAccessoryObserver::CountRemovedAccessoriesL() const
    {
    TInt removedAccessories( 0 );

    for ( TInt i = 0; i < iSavedAccessories.Count(); i++ )
        {
        TAccPolGenericID savedAcc = iSavedAccessories[i];

        TUint32 physicalConnectionCaps = savedAcc.PhysicalConnectionCaps();

        if ( physicalConnectionCaps & KPCWired || physicalConnectionCaps & KPCBluetooth )
            {
            TBool found( EFalse );

            for ( TInt j = 0; j < iAccPolGenIdArr.Count() && !found; j++ )
                {
                TAccPolGenericID freshAcc = iAccPolGenIdArr.GetGenericIDL( j );
                if ( freshAcc.UniqueID() == savedAcc.UniqueID() )
                    {
                    LOG_FORMAT( "CRadioAccessoryObserver::CountRemovedAccessoriesL() -- Accessory with id = %d found.", savedAcc.UniqueID() );
                    found = ETrue;
                    }
                }
            if ( !found )
                {
                LOG_FORMAT( "CRadioAccessoryObserver::CountRemovedAccessoriesL() -- Accessory with id = %d removed.", savedAcc.UniqueID() );
                removedAccessories++;
                }
            }
        }
    return removedAccessories;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioAccessoryObserver::CountAddedAccessoriesL() const
    {
    TInt addedAccessories( 0 );

    for ( TInt i = 0; i < iAccPolGenIdArr.Count(); i++ )
        {
        TAccPolGenericID freshAcc = iAccPolGenIdArr.GetGenericIDL( i );

        TUint32 physicalConnectionCaps = freshAcc.PhysicalConnectionCaps();

        if ( physicalConnectionCaps & KPCWired || physicalConnectionCaps & KPCBluetooth )
            {
            TBool found( EFalse );

            for ( TInt j = 0; j < iSavedAccessories.Count() && !found; j++ )
                {
                TAccPolGenericID savedAcc = iSavedAccessories[j];
                if ( savedAcc.UniqueID() == freshAcc.UniqueID() )
                    {
                    LOG_FORMAT( "CRadioAccessoryObserver::CountAddedAccessoriesL() -- Accessory with id = %d found.", freshAcc.UniqueID() );
                    found = ETrue;
                    }
                }
            if ( !found )
                {
                LOG_FORMAT( "CRadioAccessoryObserver::CountAddedAccessoriesL() -- Accessory with id = %d added.", freshAcc.UniqueID() );
                addedAccessories++;
                }
            }
        }
    return addedAccessories;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::SetObserver( MRadioHeadsetEventObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// From class CActive
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::RunL()
    {
    TRequestStatus status = iStatus;
    iAccessoryConn.NotifyAccessoryConnectionStatusChanged( iStatus, iAccPolGenIdArr );

    SetActive();

    TInt nroChangedAccessories( 0 );
    UpdateCurrentAccessoriesL( nroChangedAccessories );

    if ( status == KErrNone && nroChangedAccessories != 0 )
        {
        TAccAudioOutPutType audioOutputType( EAccAudioOutPutTypePrivate );

        if ( FindAccessoryL( EFalse, audioOutputType ) )
            {
            // Accessory mode may change when combined connection status changes
            // or when audio routing status changes.
            if ( audioOutputType == EAccAudioOutPutTypePrivate )
                {
                LOG( "CRadioAccessoryObserver::RunL - Headset connected" );
                iObserver->HeadsetConnectedCallbackL();
                }
            else
                {
                LOG( "CRadioAccessoryObserver::RunL - Public wired or BT accessory connected." );
                iObserver->HeadsetDisconnectedCallbackL();
                }
            }
        else
            {
            LOG( "CRadioAccessoryObserver::RunL - No wired or BT accessories available" );
            iObserver->HeadsetDisconnectedCallbackL();
            }
        }
    }


// ---------------------------------------------------------------------------
// From class CActive
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::DoCancel()
    {
    iAccessoryConn.CancelNotifyAccessoryConnectionStatusChanged();
    }
