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

#ifndef MRADIOSCANOBSERVER_H_
#define MRADIOSCANOBSERVER_H_

/**
 * Interface for radio station scanning.
 *
 * Implementers of this interface are able to receive notifications
 * when scanning finds a valid frequency and upon its completion.
 */
NONSHARABLE_CLASS( MRadioScanObserver )
    {

public:

    /**
     * Invoked when a frequency has been found with scanning.
     *
     * @param   aFrequency      The frequency, in kilohertz, that was found.
     */
    virtual void ScanFrequencyEventL( TUint32 aFrequency ) = 0;

    /**
     * Invoked when a scan has been completed.
     *
     * @param   aError      <code>KErrNone</code> when the scan was successfull,
     *                      or any of the standard error codes otherwise.
     */
    virtual void ScanCompletedEventL( TInt aError ) = 0;

    };

#endif // MRADIOSCANOBSERVER_H_
