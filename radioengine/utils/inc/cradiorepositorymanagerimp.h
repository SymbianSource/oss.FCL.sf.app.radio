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


#ifndef C_RADIOREPOSITORYMANAGERIMP_H
#define C_RADIOREPOSITORYMANAGERIMP_H

// System includes
#include <centralrepository.h>

// User includes
#include "mradiorepositoryentityobserver.h"

// Forward declarations
class CRadioRepositoryEntityBase;

/**
 *  Central repository entity manager.
 *
 *  Manages central repository entities that listen to specific repository and a key within it, notifying the observer
 *  whenever the key's value changes.
 *
 *  Also a set of static methods are provided that wrap the common use case of central repository where the user
 *  is only interested to read or write the value of a key and then discard the repository object.
 */
NONSHARABLE_CLASS( CRadioRepositoryManagerImp ) : public CRadioRepositoryManager
                                                , public MRadioRepositoryEntityObserver
    {

public:

    static CRadioRepositoryManagerImp* NewL( TInt aGranularity );

    ~CRadioRepositoryManagerImp();

    /**
     * Sets the value of an integer key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt aValue );

    /**
     * Sets the value of a floating point key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TReal& aValue );

    /**
     * Sets the value of a binary key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue );

    /**
     * Sets the value of a string key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue );

    /**
     * Gets the value of an integer key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt& aValue );

    /**
     * Gets the value of a floating point key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TReal& aValue );

    /**
     * Gets the value of a binary key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes8& aValue );

    /**
     * Gets the value of a string key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes16& aValue );

private:

    CRadioRepositoryManagerImp( TInt aGranularity );

    void ConstructL();

// from base class CRadioRepositoryManager

    void AddObserverL( MRadioRepositoryEntityObserver* aObserver );
    void RemoveObserver( MRadioRepositoryEntityObserver* aObserver );
    void EnableSave( TBool aEnable );
    void AddEntityL( const TUid& aUid, TUint32 aKey, TRadioEntityType aType );
    void RemoveEntity( const TUid& aUid, TUint32 aKey );
    TInt SetEntityValue( const TUid& aUid, TUint32 aKey, TInt aValue );
    TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TReal& aValue );
    TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC8& aValue );
    TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC16& aValue );
    TInt EntityValueInt( const TUid& aUid, TUint32 aKey ) const;
    const TReal& EntityValueReal( const TUid& aUid, TUint32 aKey ) const;
    const TDesC8& EntityValueDes8( const TUid& aUid, TUint32 aKey ) const;
    const TDesC16& EntityValueDes16( const TUid& aUid, TUint32 aKey ) const;

    /**
     * Sets the value of an entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The value to set.
     */
    template <typename T>
    TInt DoSetEntityValue( const TUid& aUid, TUint32 aKey, const T& aValue );

    /**
     * Returns an entity's cached value.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @return  The cached value of the entity.
     */
    template <typename T>
    const T& DoEntityValue( const TUid& aUid, TUint32 aKey ) const;

    /**
     * Sets the value of a key.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    template <typename T>
    static void DoSetRepositoryValueL( const TUid& aUid, TUint32 aKey, const T& aValue );

    /**
     * Returns a key's value.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @return  The key's value.
     */
    template <typename T>
    static void DoGetRepositoryValueL( const TUid& aUid, TUint32 aKey, T& aValue );

    /**
     * Returns the entity's index by its UID and key.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   The entity's index or <code>KErrNotFound</code> if no such entity could be found.
     */
    TInt EntityIndex( const TUid& aUid, TUint32 aKey ) const;

    /**
     * SIS installation does not update central repository, this checks that the new keys are in the repository.
     */
    void CheckRepositoryKeysL();

// from base class MRadioRepositoryEntityObserver

    void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, TInt aValue, TInt aError );
    void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TReal& aValue, TInt aError );
    void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue, TInt aError );
    void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue, TInt aError );

private: // data

    /** The observers that are notified of changes in the entities' values. */
    RPointerArray<MRadioRepositoryEntityObserver> iObservers;

    /** The entities registered. */
    RPointerArray<CRadioRepositoryEntityBase> iEntities;

    /** Indicates whether or not the entities are allowed to write to the repository. */
    TBool iEnableSave;

    };

#endif // C_RADIOREPOSITORYMANAGERIMP_H
