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
* Description:  This file contains definitions for 
*               FM Radio command line parameters
*
*/


#ifndef FMRADIOCOMMANDLINEPARAMS_H
#define FMRADIOCOMMANDLINEPARAMS_H

#include <e32std.h>

/**
 * The needed ID's are defined in fmradiouids.h
 * Only one of the following commands can be currently given at a time
 */

/**
 * FM Radio application UID
 */
const TUid KFMRadioCommandUid = {0x10207A89};

/**
 * Starts FM Radio to specified view or activates it if some other view is already active
 * Supports only Main view and Channel List View
 * Example: "-v 1", use the ID's defined in fmradiouids.h
 */
_LIT8( KFMRadioCommandActivateView, "-v" );

/**
 * Starts FM Radio to specified frequency or just tunes if application is already running. 
 * No effect if the frequency is outside current frequency range. Example: "-f 87500000"
 * @param frequency The frequency to be activated in hertz
 */
_LIT8( KFMRadioCommandFrequency, "-f" );

/**
 * Starts FM Radio to specified station index or just tunes if application is already running. 
 * No effect if the index is out of boundaries. Example: "-i 0"
 * @param index The index to be activated
 */
_LIT8( KFMRadioCommandStationIndex, "-i" );

#endif // FMRADIOCOMMANDLINEPARAMS_H

// End of file
