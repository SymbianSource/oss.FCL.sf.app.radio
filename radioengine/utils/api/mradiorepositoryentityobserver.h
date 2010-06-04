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

#ifndef M_RADIOREPOSITORYENTITYOBSERVER_H
#define M_RADIOREPOSITORYENTITYOBSERVER_H

/**
 * Entity observer interface.
 */
NONSHARABLE_CLASS( MRadioRepositoryEntityObserver )
    {

public:

    /**
     * Invoked when the observed entity's value is changed.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The new value of the entity.
     * @param   aError      One of the standard system error codes.
     */
    virtual void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, TInt aValue, TInt aError ) = 0;

    /**
     * Invoked when the observed entity's value is changed.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The new value of the entity.
     * @param   aError      One of the standard system error codes.
     */
    virtual void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TReal& aValue, TInt aError ) = 0;

    /**
     * Invoked when the observed entity's value is changed.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The new value of the entity.
     * @param   aError      One of the standard system error codes.
     */
    virtual void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue, TInt aError ) = 0;

    /**
     * Invoked when the observed entity's value is changed.
     *
     * @param   aUid        The UID of the entity.
     * @param   aKey        The key of the entity.
     * @param   aValue      The new value of the entity.
     * @param   aError      One of the standard system error codes.
     */
    virtual void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue, TInt aError ) = 0;

    };

#endif // M_RADIOREPOSITORYENTITYOBSERVER_H

