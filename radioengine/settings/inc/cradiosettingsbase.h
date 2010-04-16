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

#ifndef C_RADIOSETTINGSBASE_H
#define C_RADIOSETTINGSBASE_H

// System includes
#include <badesca.h>
#include <e32base.h>

// Forward declarations
class CCoeEnv;
class CRadioRepositoryManager;

/**
 * Base class for all settings implementations.
 *
 * Provides commonly used data members and methods.
 */
NONSHARABLE_CLASS( CRadioSettingsBase ) : public CBase
    {

public:

    ~CRadioSettingsBase();

protected:

    CRadioSettingsBase( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv );

    /**
     * Writes an array to the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key to which to write the array.
     * @param   aArray      The array to write.
     */
    void WriteArrayEntityL( const TUid& aUid, TUint32 aKey, const CDesCArray& aArray );

    /**
     * Reads an array from the repository.
     *
     * @param   aUid        The UID of the repository.
     * @param   aKey        The key to which to write the array.
     * @return  The array that was read. Ownership is transferred to the caller.
     */
    CDesCArray* ReadArrayEntityL( const TUid& aUid, TUint32 aKey ) const;

protected:

    /**
     * The control environment.
     */
    CCoeEnv&                    iCoeEnv;

    /**
     * The repository manager.
     */
    CRadioRepositoryManager&    iRepositoryManager;

    };

#endif // C_RADIOSETTINGSBASE_H
