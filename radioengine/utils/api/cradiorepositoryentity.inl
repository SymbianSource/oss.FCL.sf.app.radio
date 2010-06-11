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
#include <centralrepository.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
EXPORT_C CRadioRepositoryEntity<T>* CRadioRepositoryEntity<T>::NewL( const TUid& aUid,
                                                            TUint32 aKey,
                                                            MRadioRepositoryEntityObserver& aObserver,
                                                            CActive::TPriority aPriority )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioRepositoryEntity<T>* self = new ( ELeave ) CRadioRepositoryEntity<T>( aUid, aKey, aObserver, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
CRadioRepositoryEntity<T>::CRadioRepositoryEntity( const TUid& aUid,
                                                   TUint32 aKey,
                                                   MRadioRepositoryEntityObserver& aObserver,
                                                   CActive::TPriority aPriority )
    : CRadioRepositoryEntityBase( aUid, aKey, aObserver, aPriority )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRepository = CRepository::NewL( iUid );

    CActiveScheduler::Add( this );
    iRepository->Get( iKey, iValue );
    User::LeaveIfError( iRepository->NotifyRequest( iKey, iStatus ) );
    SetActive();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
EXPORT_C CRadioRepositoryEntity<T>::~CRadioRepositoryEntity()
    {
    LEVEL3( LOG_METHOD_AUTO );
    Cancel();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// Sets the value of the key.
// -----------------------------------------------------------------------------
//
template <typename T>
TInt CRadioRepositoryEntity<T>::SetValue( const T& aValue, TBool aSavingEnabled )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iValue = aValue;
    if ( aSavingEnabled )
        {
        return iRepository->Set( iKey, aValue );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Returns the cached copy of the key's value.
// -----------------------------------------------------------------------------
//
template <typename T>
const T& CRadioRepositoryEntity<T>::Value() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iValue;
    }

// -----------------------------------------------------------------------------
// Forcibly updates the cached value from the repository.
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::UpdateL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    User::LeaveIfError( iRepository->Get( iKey, iValue ) );
    }

// -----------------------------------------------------------------------------
// Executed when the key's value is changed.
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::RunL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    User::LeaveIfError( iRepository->NotifyRequest( iKey, iStatus ) );
    SetActive();

    TInt err = iRepository->Get( iKey, iValue );
    iObserver.HandleRepositoryValueChangeL( iUid, iKey, iValue, err );
    }

// -----------------------------------------------------------------------------
// Cancels all pending notifications.
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::DoCancel()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRepository->NotifyCancel( iKey );
    }
