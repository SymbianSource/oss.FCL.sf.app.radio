/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  headset event observer interface
*
*/


#ifndef MFMRADIOHEADSETEVENTOBSERVER_H
#define MFMRADIOHEADSETEVENTOBSERVER_H


// CLASS DECLARATION

class MFMRadioHeadsetEventObserver 
    {

    public: // New functions

		/**
		* This callback will notify of the Headset to be connected
		*/        
        virtual void HeadsetAccessoryConnectedCallbackL() = 0;

		/**
		* This callback will notify of the Headset to be disconnected
		*/
		virtual void HeadsetAccessoryDisconnectedCallbackL() = 0;
    };

#endif      // MFMRADIOHEADSETEVENTOBSERVER_H
            
// End of File
