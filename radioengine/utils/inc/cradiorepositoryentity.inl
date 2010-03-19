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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
CRadioRepositoryEntity<T>* CRadioRepositoryEntity<T>::NewL( const TUid& aUid,
                                                            TUint32 aKey,
                                                            MRadioRepositoryEntityObserver& aObserver,
                                                            CActive::TPriority aPriority )
    {
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
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::ConstructL()
    {
    iRepository = CRepository::NewL( iUid );

    CActiveScheduler::Add( this );
    RunL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
template <typename T>
CRadioRepositoryEntity<T>::~CRadioRepositoryEntity()
    {
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
    return iValue;
    }

// -----------------------------------------------------------------------------
// Forcibly updates the cached value from the repository.
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::UpdateL()
    {
    User::LeaveIfError( iRepository->Get( iKey, iValue ) );
    }

// -----------------------------------------------------------------------------
// Executed when the key's value is changed.
// -----------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryEntity<T>::RunL()
    {
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
    iRepository->NotifyCancel( iKey );
    }
