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

#ifndef C_RADIOREPOSITORYENTITYBASE_H
#define C_RADIOREPOSITORYENTITYBASE_H

#include <e32base.h>

class MRadioRepositoryEntityObserver;

/**
 * Base class for a central repository entity.
 *
 * All instantiable central repository entities must be derived from this class.
 * The class is provided so that a heterogenous array of templated objects can be created, and to
 * provide common data fields for the said purpose.
 */
NONSHARABLE_CLASS( CRadioRepositoryEntityBase ) : public CActive
    {

public:

    ~CRadioRepositoryEntityBase();

    /**
     * Returns the UID of the repository the entity is bound to.
     *
     * @return  The UID of the repository the entity is bound to.
     */
    const TUid& Uid() const;

    /**
     * Returns the key the entity is bound to.
     *
     * @return  The key the entity is bound to.
     */
    TUint32 Key() const;

protected:

    /**
     * Constructor.
     *
     * @param   aUid        The UID of the repository that this entity will be bound to.
     * @param   aKey        The key within the repository that this entity will be bound to.
     * @param   aObserver   The observer that is notified whenever the value of the key within the repository changes.
     * @param   aPriority   The priority of the active object.
     */
    CRadioRepositoryEntityBase( const TUid& aUid, TUint32 aKey, MRadioRepositoryEntityObserver& aObserver, CActive::TPriority aPriority );

protected:

    /** The observer that is notified of changes in the key. */
    MRadioRepositoryEntityObserver& iObserver;

    /** The UID of the repository the entity is bound to. */
    TUid iUid;

    /** The key the entity is bound to.. */
    TUint32 iKey;

    };

#endif // C_RADIOREPOSITORYENTITYBASE_H
