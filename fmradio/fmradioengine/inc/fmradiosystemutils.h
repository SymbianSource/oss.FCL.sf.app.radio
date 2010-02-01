/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FM Radio related definitions
*
*/

#ifndef FMRADIOSYSTEMUTILS_H
#define FMRADIOSYSTEMUTILS_H

#include <e32std.h>

/** Direction of tuning */
enum TFMRadioTuneDirection
    {
    EFMRadioDown, 
    EFMRadioUp 
    };

/** Direction when setting the volume */
enum TFMRadioVolumeSetDirection
    {
    EFMRadioDecVolume,
    EFMRadioIncVolume
    };

#endif // FMRADIOSYSTEMUTILS_H


