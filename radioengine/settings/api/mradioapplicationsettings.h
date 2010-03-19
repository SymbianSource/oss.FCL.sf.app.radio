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

#ifndef M_RADIOAPPLICATIONSETTINGS_H
#define M_RADIOAPPLICATIONSETTINGS_H

#include <badesca.h>

/**
 * MRadioApplicationSettings
 *
 * Interface for managing persistent Visual Radio application settings.
 */
NONSHARABLE_CLASS( MRadioApplicationSettings )
    {

public:

    /**
     * Possible states for audio playing state.
     * Once the state reaches ´played´, it doesn't change.
     */
    enum TRadioCRAudioPlayHistory
        {
        ERadioCRInitial,
        ERadioCRAudioTriggered,
        ERadioCRAudioPlayed
        };

public:

    /**
     * Stores the currently active preset in the preset list.
     *
     * @param aIndex The list box index of the preset that is
     * currently active.
     */
    virtual TInt SetActiveFocusLocation( TInt aIndex ) = 0;

    /**
     * Retrieves the list box index of the currently active preset.
     * By default, the first preset is active.
     *
     * @return The list box index of the currently active preset.
     */
    virtual TInt ActiveFocusLocation() const = 0;

    /**
     * Stores the audio history telling whether the audio has been
     * played ( so that user has triggered it ).
     *
     * Once the history reaches state 'played', it does not change.
     *
     * @param aHistory The audio history.
     */
    virtual void SetAudioPlayHistoryL( TRadioCRAudioPlayHistory aHistory ) = 0;

    /**
     * Retrieves the current audio play history.
     *
     * @return The current audio play history.
     */
    virtual TRadioCRAudioPlayHistory AudioPlayHistory() const = 0;

    virtual TInt UpdateStartCount() = 0;

    virtual TInt SetUiFlags( TUint aUiFlags ) = 0;

    virtual TUint UiFlags() const = 0;

    };

#endif // M_RADIOAPPLICATIONSETTINGS_H
