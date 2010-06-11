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

#ifndef RADIOENGINEUTILS_H
#define RADIOENGINEUTILS_H

// System includes
#include <e32std.h>

// User includes

// Forward declarations
class MRadioEngineLogger;
class CCoeEnv;
class RFs;

/**
 * System utility collection class.
 */
class RadioEngineUtils
    {

public:

    IMPORT_C static void InitializeL();

    IMPORT_C static void Release();

    IMPORT_C static MRadioEngineLogger* Logger();

    /**
     * Returns the file server session
     *
     * @return Reference to file server session
     */
    IMPORT_C static RFs& FsSession();

    /**
     * Converts a frequency to a descriptor format.
     * Returned descriptor is left in CleanupStack.
     *
     * @param   aFreq           Frequency, in kilohertz, to convert.
     * @param   aDecimalCount   Decimal count to use.
     * @param   aResourceId     Resource ID of the format string, or <code>KErrNotFound</code> if no such resource is to be used.
     * @return  The formatted descriptor. Ownership is transferred.
     */
    IMPORT_C static HBufC* ReadFrequencyStringLC( TUint32 aFreq, TInt aDecimalCount, TInt aResourceId = KErrNotFound );

    /**
     * Writes formatted frequency to a give descriptor.
     *
     * @param   aDest           Destination descriptor.
     * @param   aFreq           Frequency, in kilohertz, to convert.
     * @param   aDecimalCount   Decimal count to use.
     * @param   aFormat         Format read from resource.
     */
    IMPORT_C static void FormatFrequencyString( TDes& aDest, TUint32 aFreq, TInt aDecimalCount, TDesC& aFormat );

    };

#endif // RADIOENGINEUTILS_H
