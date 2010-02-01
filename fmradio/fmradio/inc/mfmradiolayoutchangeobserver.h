/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares an interface for observing
*
*/



#ifndef MFMRADIOLAYOUTCHANGEOBSERVER_H
#define MFMRADIOLAYOUTCHANGEOBSERVER_H

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Notifies layot change.
*
*  @lib VoiceRecorderUtils.dll
*  @since 2.0
*/
class MFMRadioLayoutChangeObserver
    {
    public:
		virtual void LayoutChangedL( TInt aType ) = 0;
    };

#endif      // MFMRADIOLAYOUTCHANGEOBSERVER_H

// End of File
