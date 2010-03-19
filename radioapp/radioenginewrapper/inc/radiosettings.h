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

#ifndef RADIOSETTINGS_H_
#define RADIOSETTINGS_H_

// User includes
#include "radiowrapperexport.h"

// Forward declarations
class RadioSettingsPrivate;

// Class declaration
class WRAPPER_DLL_EXPORT RadioSettings
    {
    Q_DECLARE_PRIVATE_D( d_ptr, RadioSettings )
    Q_DISABLE_COPY( RadioSettings )

    friend class RadioEngineWrapperPrivate;

public:

    // First time start
    bool isFirstTimeStart();

    // Favorites
    bool showFavorites() const;
    void setShowFavorites( bool showFavorites );
    bool toggleShowFavorites();

private:

    explicit RadioSettings();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioSettingsPrivate* const d_ptr;

    };

#endif // RADIOSETTINGS_H_
