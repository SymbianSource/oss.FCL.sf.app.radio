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

#ifndef C_RADIOREPOSITORYENTITY_H
#define C_RADIOREPOSITORYENTITY_H

// User includes
#include "cradiorepositoryentitybase.h"
#include "mradiorepositoryentityobserver.h"
#include "cradioenginelogger.h"

// Forward declarations
class CRepository;

/**
 * Templated class to listen to a single key within the central repository.
 *
 * Notifies the observer of changes in the key, and provides caching for the key's value.
 */
template <typename T>
NONSHARABLE_CLASS( CRadioRepositoryEntity ) : public CRadioRepositoryEntityBase
    {

public:

    IMPORT_C static CRadioRepositoryEntity<T>* NewL( const TUid& aUid, 
                                            TUint32 aKey, 
                                            MRadioRepositoryEntityObserver& aObserver, 
                                            CActive::TPriority aPriority = CActive::EPriorityStandard );

    IMPORT_C ~CRadioRepositoryEntity();

    /**
     * Sets the value of the entity.
     * The value is written into the central repository immediately.
     *
     * @param   aValue  The value to set.
     * @param   aSavingEnabled If ETrue, saving to central repository file is enabled
     */
    TInt SetValue( const T& aValue, TBool aSavingEnabled );

    /**
     * Returns a cached reference to the value of the entity.
     *
     * @return  The cached value of the entity.
     */
    const T& Value() const;

    /**
     * Forcibly updates the key's value from the central repository and caches it.
     */
    void UpdateL();

// from base class CActive

    void RunL();
    void DoCancel();

private:

    CRadioRepositoryEntity( const TUid& aUid, 
                            TUint32 aKey, 
                            MRadioRepositoryEntityObserver& aObserver, 
                            CActive::TPriority aPriority );

    void ConstructL();

protected:

    /** The central repository client. */
    CRepository*    iRepository;
    
    /** The cached value of the key. */
    T               iValue;

    };

#include "cradiorepositoryentity.inl"

#endif // C_RADIOREPOSITORYENTITY_H
