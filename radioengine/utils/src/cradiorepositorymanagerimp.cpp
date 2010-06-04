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

#include "cradiorepositoryentity.h"
#include "cradiorepositorymanager.h"
#include "cradiorepositorymanagerimp.h"
#include "radioengineutils.h"

const TInt KVRRepositoryObserverArrayGranularity = 2;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Panics the application.
// ---------------------------------------------------------------------------
//
void Panic( TInt aReason )
    {
    _LIT( category, "CRadioRepositoryManagerImp" );
    User::Panic( category, aReason );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRepositoryManagerImp* CRadioRepositoryManagerImp::NewL( TInt aGranularity )
    {
    CRadioRepositoryManagerImp* self = new ( ELeave ) CRadioRepositoryManagerImp( aGranularity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    CheckRepositoryKeysL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRepositoryManagerImp::CRadioRepositoryManagerImp( TInt aGranularity )
    : iObservers( KVRRepositoryObserverArrayGranularity )
    , iEntities( aGranularity )
    , iEnableSave( ETrue )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRepositoryManagerImp::~CRadioRepositoryManagerImp()
    {
    iObservers.Close();
    iEntities.ResetAndDestroy();
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
// Adds observer
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::AddObserverL( MRadioRepositoryEntityObserver* aObserver )
    {
    iObservers.AppendL( aObserver );
    }

// ---------------------------------------------------------------------------
// Removes observer
// ---------------------------------------------------------------------------
//
 void CRadioRepositoryManagerImp::RemoveObserver( MRadioRepositoryEntityObserver* aObserver )
    {
    TInt objectIndex = iObservers.Find( aObserver );

    if ( objectIndex != KErrNotFound )
        {
        iObservers.Remove( objectIndex );
        }
    }

// ---------------------------------------------------------------------------
// Enables or disables the saving of entities.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::EnableSave( TBool aEnable )
    {
    iEnableSave = aEnable;
    }

// ---------------------------------------------------------------------------
// Adds an entity.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::AddEntityL( const TUid& aUid, TUint32 aKey, TRadioEntityType aType )
    {
    CRadioRepositoryEntityBase* entity = NULL;

    switch ( aType )
        {
        case ERadioEntityInt:
            entity = CRadioRepositoryEntity<TInt>::NewL( aUid, aKey, *this );
            break;
        case ERadioEntityReal:
            entity = CRadioRepositoryEntity<TReal>::NewL( aUid, aKey, *this );
            break;
        case ERadioEntityDes8:
            entity = CRadioRepositoryEntity<TRadioEntityBuf8>::NewL( aUid, aKey, *this );
            break;
        case ERadioEntityDes16:
            entity = CRadioRepositoryEntity<TRadioEntityBuf16>::NewL( aUid, aKey, *this );
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    CleanupStack::PushL( entity );
    iEntities.AppendL( entity );
    CleanupStack::Pop( entity );
    }

// ---------------------------------------------------------------------------
// Removes an entity.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::RemoveEntity( const TUid& aUid, TUint32 aKey )
    {
    TInt idx = EntityIndex( aUid, aKey );

    if ( idx >= 0 )
        {
        CRadioRepositoryEntityBase* entity = iEntities[idx];
        iEntities.Remove( idx );
        iEntities.GranularCompress();
        delete entity;
        }
    }

// ---------------------------------------------------------------------------
// Sets an entity's value.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::SetEntityValue( const TUid& aUid, TUint32 aKey, TInt aValue )
    {
    return DoSetEntityValue<TInt>( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets an entity's value.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::SetEntityValue( const TUid& aUid, TUint32 aKey, const TReal& aValue )
    {
    return DoSetEntityValue<TReal>( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets an entity's value.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC8& aValue )
    {
    return DoSetEntityValue<TRadioEntityBuf8>( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets an entity's value.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC16& aValue )
    {
    return DoSetEntityValue<TRadioEntityBuf16>( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Returns an entity's value.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::EntityValueInt( const TUid& aUid, TUint32 aKey ) const
    {
    return DoEntityValue<TInt>( aUid, aKey );
    }

// ---------------------------------------------------------------------------
// Returns an entity's value.
// ---------------------------------------------------------------------------
//
const TReal& CRadioRepositoryManagerImp::EntityValueReal( const TUid& aUid, TUint32 aKey ) const
    {
    return DoEntityValue<TReal>( aUid, aKey );
    }

// ---------------------------------------------------------------------------
// Returns an entity's value.
// ---------------------------------------------------------------------------
//
const TDesC8& CRadioRepositoryManagerImp::EntityValueDes8( const TUid& aUid, TUint32 aKey ) const
    {
    return DoEntityValue<TDesC8>( aUid, aKey );
    }

// ---------------------------------------------------------------------------
// Returns an entity's value.
// ---------------------------------------------------------------------------
//
const TDesC16& CRadioRepositoryManagerImp::EntityValueDes16( const TUid& aUid, TUint32 aKey ) const
    {
    return DoEntityValue<TDesC16>( aUid, aKey );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt aValue )
    {
    DoSetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TReal& aValue )
    {
    DoSetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue )
    {
    DoSetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue )
    {
    DoSetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt& aValue )
    {
    DoGetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TReal& aValue )
    {
    DoGetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes8& aValue )
    {
    DoGetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes16& aValue )
    {
    DoGetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets an entity's value.
// ---------------------------------------------------------------------------
//
template <typename T>
TInt CRadioRepositoryManagerImp::DoSetEntityValue( const TUid& aUid, TUint32 aKey, const T& aValue )
    {
    TInt idx = EntityIndex( aUid, aKey );

    // If the entity could not be found, panic the application.
    __ASSERT_ALWAYS( idx >= 0, ::Panic( KErrArgument ) );

    return static_cast<CRadioRepositoryEntity<T>*>( iEntities[idx] )->SetValue( aValue, iEnableSave );
    }

// ---------------------------------------------------------------------------
// Returns an entity's value.
// ---------------------------------------------------------------------------
//
template <typename T>
const T& CRadioRepositoryManagerImp::DoEntityValue( const TUid& aUid, TUint32 aKey ) const
    {
    TInt idx = EntityIndex( aUid, aKey );

    // If the entity could not be found, panic the application.
    __ASSERT_ALWAYS( idx >= 0, ::Panic( KErrArgument ) );

    return static_cast<CRadioRepositoryEntity<T>*>( iEntities[idx] )->Value();
    }

// ---------------------------------------------------------------------------
// Sets a key's value in the repository.
// ---------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryManagerImp::DoSetRepositoryValueL( const TUid& aUid, TUint32 aKey, const T& aValue )
    {
    CRepository* repository = CRepository::NewLC( aUid );
    User::LeaveIfError( repository->Set( aKey, aValue ) );
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// Returns a key's value in the repository.
// ---------------------------------------------------------------------------
//
template <typename T>
void CRadioRepositoryManagerImp::DoGetRepositoryValueL( const TUid& aUid, TUint32 aKey, T& aValue )
    {
    CRepository* repository = CRepository::NewLC( aUid );
    User::LeaveIfError( repository->Get( aKey, aValue ) );
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// Returns the entity's index that matches the supplied arguments.
// ---------------------------------------------------------------------------
//
TInt CRadioRepositoryManagerImp::EntityIndex( const TUid& aUid, TUint32 aKey ) const
    {
    TInt idx = KErrNotFound;

    for ( TInt i = 0; i < iEntities.Count(); i++ )
        {
        CRadioRepositoryEntityBase* entity = iEntities[i];
        if ( entity->Uid() == aUid && entity->Key() == aKey )
            {
            idx = i;
            break;
            }
        }

    return idx;
    }

// ---------------------------------------------------------------------------
// SIS installation does not update central repository, this checks that the new keys are in the repository.
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::CheckRepositoryKeysL()
    {
    // After adding a new cenrep key to radiointernalcrkeys.h, add it also here with a default value
    /*
    CRepository* repository = CRepository::NewLC( KVRCRUid );

    TInt ret = repository->Create( KVRCRNewValue, TInt( 123 ) );
    if ( ret != KErrAlreadyExists )
        {
        User::LeaveIfError( ret );
        }

    CleanupStack::PopAndDestroy( repository );
    */
    }

// ---------------------------------------------------------------------------
// Forwards the repository value changes
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::HandleRepositoryValueChangeL( const TUid& aUid,
        TUint32 aKey, TInt aValue, TInt aError )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleRepositoryValueChangeL( aUid, aKey, aValue, aError );
        }
    }

// ---------------------------------------------------------------------------
// Forwards the repository value changes
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::HandleRepositoryValueChangeL( const TUid& aUid,
        TUint32 aKey, const TReal& aValue, TInt aError )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleRepositoryValueChangeL( aUid, aKey, aValue, aError );
        }
    }

// ---------------------------------------------------------------------------
// Forwards the repository value changes
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::HandleRepositoryValueChangeL( const TUid& aUid,
        TUint32 aKey, const TDesC8& aValue, TInt aError )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleRepositoryValueChangeL( aUid, aKey, aValue, aError );
        }
    }

// ---------------------------------------------------------------------------
// Forwards the repository value changes
// ---------------------------------------------------------------------------
//
void CRadioRepositoryManagerImp::HandleRepositoryValueChangeL( const TUid& aUid,
        TUint32 aKey, const TDesC16& aValue, TInt aError )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleRepositoryValueChangeL( aUid, aKey, aValue, aError );
        }
    }
