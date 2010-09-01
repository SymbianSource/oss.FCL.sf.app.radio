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
* Description:  Declares an interface for handling the media idle components
*
*/

#ifndef FMRADIOIDLECONTROLINTERFACE_H_
#define FMRADIOIDLECONTROLINTERFACE_H_

/*
 * Interface for handling the media idle components.
 * Media idle visuals are controlled solely through this interface.
 */
class MFMRadioIdleControlInterface
    {
    public:
        /*
         * Show the component with fade in 
         */
        virtual void Show() = 0;
        
        /*
         * Hide the component with fade out
         */
        virtual void Hide() = 0;
        
        /*
         * Deactivates control. Used for example
         * foreground and background events. 
         */
        virtual void Deactivate() = 0;
    };


#endif /* FMRADIOIDLECONTROLINTERFACE_H_ */
