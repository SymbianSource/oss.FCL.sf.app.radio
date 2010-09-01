/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains implementation of the CDosServerObserver class. which is the 
*                       notification interface to the DOS Server.
*
*/

#ifndef __DOSSERVEROBSERVER_H
#define __DOSSERVEROBSERVER_H

//  INCLUDES
#include <DosSvrServices.h>

#include "fmradioengineradiosettings.h"

class CRadioEngine;

// CLASS DECLARATION

/**
* This class implements theis the notification interface to the DOS Server.
*/
class CDosServerObserver : public CDosEventListenerBase
    {
	public:
		virtual ~CDosServerObserver();
		/**
		* Two-phased class constructor.
		* @param aEngine - pointer to the CRadioEngine instance (used for callbacks)
		* @param aRadioSettings - pointer to the CRadioSettings class
		* @return pointer to CDosServerObserver class instance
		*/
		static CDosServerObserver* NewL(CRadioEngine* aEngine, TRadioSettings* aRadioSettings);

        /**
        * Sets the current audio routing.
		* @param aAudioOutput Audio output to be used
		* @return Symbian error code
        */
 
        TInt SetAudioRouting(CRadioEngine::TFMRadioAudioOutput aAudioOutput);
	
		/**
		* From CDosEventListenerBase 
		* It notifies about a headset button state change
		* @param aState State of button
		*/
		void HeadsetButtonChangedL(EPSButtonState aState);

	private:
		/**
		* Default class constructor.
		* @param aEngine - pointer to the CRadioEngine instance (used for callbacks)
		* @param aRadioSettings - pointer to the CRadioSettings class
		*/
		CDosServerObserver(CRadioEngine* aEngine, TRadioSettings* aRadioSettings);
		/**
		* Second phase class constructor.
		*/
		void ConstructL();

	private:
		CRadioEngine *iEngine;
		TRadioSettings *iRadioSettings;
        RDosAudio iDosAudio;
    };

#endif  // __DOSSERVEROBSERVER_H

// End of file
