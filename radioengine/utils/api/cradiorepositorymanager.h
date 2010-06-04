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

#ifndef C_RADIOREPOSITORYMANAGER_H
#define C_RADIOREPOSITORYMANAGER_H

// System includes
#include <e32base.h>

// Forward declarations
class MRadioRepositoryEntityObserver;

/** The maximum length of a string buffer. */
const TInt KRadioEntityBuf16Length = 256; // Maximum: NCentralRepositoryConstants::KMaxUnicodeStringLength;
/** The maximum length of a binary buffer. */
const TInt KRadioEntityBuf8Length = KRadioEntityBuf16Length * 2; // Maximum: NCentralRepositoryConstants::KMaxBinaryLength;

/** Maximum length binary buffer. */
typedef TBuf8<KRadioEntityBuf8Length> TRadioEntityBuf8;
/** Maximum length string buffer. */
typedef TBuf16<KRadioEntityBuf16Length> TRadioEntityBuf16;

/**
 *  Central repository entity manager.
 *
 *  Manages central repository entities that listen to specific repository and a key within it, notifying the observer
 *  whenever the key's value changes.
 *
 *  Also a set of static methods are provided that wrap the common use case of central repository where the user
 *  is only interested to read or write the value of a key and then discard the repository object.
 */
NONSHARABLE_CLASS( CRadioRepositoryManager ) : public CBase
    {

public:

    /**
     * Possible entity types.
     */
    enum TRadioEntityType
        {
        ERadioEntityInt,   /**< The entity is an integer. */
        ERadioEntityReal,  /**< The entity is a floating point number. */
        ERadioEntityDes8,  /**< The entity is a binary value. */
        ERadioEntityDes16  /**< The entity is a string. */
        };

    IMPORT_C static CRadioRepositoryManager* NewL( TInt aGranularity );

    IMPORT_C ~CRadioRepositoryManager();

    /**
     * Adds observer
     *
     * @param aObserver The observer that is notified of all the changes in the observed keys' values.
     */
    virtual void AddObserverL( MRadioRepositoryEntityObserver* aObserver ) = 0;

    /**
     * Removes observer
     *
     * @param aObserver The observer that is to be removed
     */
    virtual void RemoveObserver( MRadioRepositoryEntityObserver* aObserver ) = 0;

    /**
     * Enables or disables the writing of entities' values to the repository.
     *
     * Disabling the save functionality is important when the free disk space reaches critical level.
     *
     * @param   aEnable     <code>ETrue</code> if the entities are allowed to write their value to the repository, <code>EFalse</code> otherwise.
     */
    virtual void EnableSave( TBool aEnable ) = 0;

    /**
     * Adds an entity to the manager.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aType       The type of the key within the repository.
     */
    virtual void AddEntityL( const TUid& aUid, TUint32 aKey, TRadioEntityType aType ) = 0;

    /**
     * Removes an entity from the manager.
     *
     * Entities should only be removed when they are no longer needed. The destructor of the repository manager
     * deletes all the remaining entities.
     *
     * @param   aUid        The UID of the entity to be removed.
     * @param   aKey        The key of the entity to be removed.
     */
    virtual void RemoveEntity( const TUid& aUid, TUint32 aKey ) = 0;

    /**
     * Sets the value of an integer entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The value to set.
     */
    virtual TInt SetEntityValue( const TUid& aUid, TUint32 aKey, TInt aValue ) = 0;

    /**
     * Sets the value of a floating point entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The value to set.
     */
    virtual TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TReal& aValue ) = 0;

    /**
     * Sets the value of a binary entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The value to set.
     */
    virtual TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC8& aValue ) = 0;

    /**
     * Sets the value of a string entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The value to set.
     */
    virtual TInt SetEntityValue( const TUid& aUid, TUint32 aKey, const TDesC16& aValue ) = 0;

    /**
     * Returns the cached value of an integer entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @return  The cached value of the entity.
     */
    virtual TInt EntityValueInt( const TUid& aUid, TUint32 aKey ) const = 0;

    /**
     * Returns the cached value of a floating point entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @return  The cached value of the entity.
     */
    virtual const TReal& EntityValueReal( const TUid& aUid, TUint32 aKey ) const = 0;

    /**
     * Returns the cached value of a binary entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @return  The cached value of the entity.
     */
    virtual const TDesC8& EntityValueDes8( const TUid& aUid, TUint32 aKey ) const = 0;

    /**
     * Returns the cached value of a string entity.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @return  The cached value of the entity.
     */
    virtual const TDesC16& EntityValueDes16( const TUid& aUid, TUint32 aKey ) const = 0;

    /**
     * Sets the value of an integer key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    IMPORT_C static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt aValue );

    /**
     * Sets the value of a floating point key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    IMPORT_C static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TReal& aValue );

    /**
     * Sets the value of a binary key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    IMPORT_C static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue );

    /**
     * Sets the value of a string key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      The value to set.
     */
    IMPORT_C static void SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue );

    /**
     * Gets the value of an integer key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    IMPORT_C static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt& aValue );

    /**
     * Gets the value of a floating point key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    IMPORT_C static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TReal& aValue );

    /**
     * Gets the value of a binary key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    IMPORT_C static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes8& aValue );

    /**
     * Gets the value of a string key in the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key within the repository.
     * @param   aValue      On return, the value of the key within the repository.
     */
    IMPORT_C static void GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes16& aValue );

    };

#endif // C_RADIOREPOSITORYMANAGER_H
