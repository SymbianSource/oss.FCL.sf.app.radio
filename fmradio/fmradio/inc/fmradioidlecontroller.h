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
* Description:  fmradio idle area controller
*
*/

#ifndef CMRADIOIDLECONTROLLER_H
#define CMRADIOIDLECONTROLLER_H

// INCLUDES
#include <e32base.h> 

// FORWARD DECLARATIONS
class MFMRadioIdleControlInterface;
// CONSTANTS

/**
 * fmradio idle area controller
 */
class CFMRadioIdleController : public CBase
    {

    public:         
        /**
         * Two-phased constructor.
        */
        static CFMRadioIdleController* NewL();
		
        /**
         * Destructor.
         */
        virtual ~CFMRadioIdleController();
            
    public: // New functions
        /**
         * Adds new item to control.
         * @aControl control to add
         */
        void AddControlItem( const MFMRadioIdleControlInterface& aControl );
        
        /**
         * Shows a control and hides all other controls
         * @param aControl control to show
         */
        void ShowControl( const MFMRadioIdleControlInterface& aControl );
        
        /**
         * Deactivates all controlled items
         */
        void DeactivateControls();
        
        /**
         * Hide all controlled items
         */
        void HideAllControls();
        
    private:
        /**
         * C++ default constructor.
         */        
        CFMRadioIdleController();

        /**
        * EPOC default constructor.
        */        
        void ConstructL();
        
        
    private: // data
        // array of controlled items
        RPointerArray<MFMRadioIdleControlInterface> iControlArray;


    };


#endif // CMRADIOIDLECONTROLLER_H
