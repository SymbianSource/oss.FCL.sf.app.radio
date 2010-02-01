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
* Description:  fmradio preset station information
*
*/

#ifndef CMFRADIOPRESET_H
#define CMFRADIOPRESET_H

// INCLUDES
#include <e32base.h> 

// FORWARD DECLARATIONS

// CONSTANTS

/**
 * fmradio preset station information
 */
class CFMRadioPreset : public CBase
    {

    public:         
        /**
         * Two-phased constructor.
        */
        static CFMRadioPreset* NewL();       
        /**
         * Destructor.
         */
        virtual ~CFMRadioPreset();
            
    public: // New functions
        /*
         * Set name for the preset
         * @param aName new name for the preset
         */
        void SetPresetNameL( const TDesC& aName );
        /*
         * Returns preset name 
         * @return name
         */
        TDesC& PresetName();
        /*
         * Set of the preset valid
         * @param aValid ETrue is the name is set valid
         */
        void SetPresetNameValid( TBool aValid );
        /*
         * Returns the validity of the preset name
         * @return ETrue if the name is valid, otherwise EFalse
         */
         TBool PresetNameValid();
        /*
         * Set frequency for the preset
         * @param aFrequency frequency to set
         */
        void SetPresetFrequency( TInt aFrequency );
        /*
         * Returns preset frequency
         * @return frequency
         */
        TInt PresetFrequency();
        
    private:
        /**
         * C++ default constructor.
         */        
        CFMRadioPreset();
        /**
        * EPOC default constructor.
        */        
        void ConstructL();
        
    private: // data
         // station name
         RBuf iStationName;
		 // station frequency
         TInt iStationFrequency;
         // Validity of the station name
         TBool iNameIsValid;
    };


#endif // CMFRADIOPRESET_H
