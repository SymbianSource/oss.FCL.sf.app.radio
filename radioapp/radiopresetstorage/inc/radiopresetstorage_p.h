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

#ifndef RADIOPRESETSTORAGE_P_H_
#define RADIOPRESETSTORAGE_P_H_

// System includes
#ifndef COMPILE_WITH_NEW_PRESET_UTILITY
#   include <radiopresetutility.h>
#endif // COMPILE_WITH_NEW_PRESET_UTILITY

#include <qscopedpointer>
#include <e32std.h>

// User includes

// Forward declarations
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    class CPresetUtility;
    typedef QScopedPointer<CPresetUtility> PresetUtilityPtr;
#else
    class CRadioFmPresetUtility;
    typedef QScopedPointer<CRadioFmPresetUtility> PresetUtilityPtr;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY

class RadioPresetStoragePrivate
#ifndef COMPILE_WITH_NEW_PRESET_UTILITY
                                : public MRadioPresetObserver
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
{
public:

    RadioPresetStoragePrivate();
    ~RadioPresetStoragePrivate();

    bool init();

#ifndef COMPILE_WITH_NEW_PRESET_UTILITY
private:
    void MrpeoPresetChanged( TPresetChangeEvent /*aChange*/, TInt /*aIndex*/ ) {}
#endif // COMPILE_WITH_NEW_PRESET_UTILITY

public: // data

    PresetUtilityPtr   mPresetUtility;

};

#endif // RADIOPRESETSTORAGE_P_H_
