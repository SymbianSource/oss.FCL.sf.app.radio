/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioRegion
*
*/


#ifndef FMRADIOREGION_H
#define FMRADIOREGION_H

//  INCLUDES
#include <e32base.h>
#include "fmradioengine.hrh"

// FORWARD DECLARATIONS
class CFMRadioRegion;
class TResourceReader;

// DATA TYPES
typedef RPointerArray<CFMRadioRegion> RFMRadioRegionArray;
typedef RPointerArray<HBufC16> RFMRadioCountryCodeArray;

// CLASS DECLARATION

/**
*  Class CFMRadioRegion provides information about FM region
*/
NONSHARABLE_CLASS( CFMRadioRegion ) : public CBase
    {
    public:  // Methods

	// Constructors and destructor
        
        /**
        * Static constructor.
        */
        static CFMRadioRegion* NewL(TResourceReader& aRr);
        
        /**
        * Destructor.
        */
         ~CFMRadioRegion();


	// New methods

        /**
        * Id Get the id of region
        * @return id of region
        */
		TFMRadioRegionSetting Id() const;

		/**
        * StepSize Get the step interval of region
        * @return Step size
        */
		TUint32 StepSize() const;

		/**
        * Get the minimum frequency of region
        * @return Minimum frequency
        */
		TUint32 MinFrequency() const;

		/**
        * Get the maximum frequency of region
        * @return Maximum frequency
        */
		TUint32 MaxFrequency() const;
		
		/**
        * Get the decimal count of region.
        * @return Decimal count.
        */
		TInt DecimalCount() const;

		/**
        * CountryCode Get the country code of region
        * @return Country code, or empty descriptor if country code doesn't exist
        */
		const RFMRadioCountryCodeArray& CountryCodes();

		/**
        * Name Get the name of region
        * @return Name of region
        */
	 	const TDesC16& Name() const;
        
        /**
        * SettingItemName Get the name of region for setting item
        * @return Name of region
        */
		const TDesC16& SettingItemName() const;

    private: //Methods

        /**
        * C++ default constructor.
        */
    	CFMRadioRegion();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TResourceReader& aRr);


    private:    // Data
		
    /** Id of the region*/
		TFMRadioRegionSetting		iId;
		
		/** Step interval*/
		TUint16				iStepSize;
		
		/** Min frequency */
		TUint32				iMinFreq;
		
		/** Max frequency */
		TUint32				iMaxFreq;
		
		/** Decimal count */
		TInt				iDecimalCount;
		
		/** Country code*/
		RFMRadioCountryCodeArray iCountryCodes;
		
		/** Region name*/
		HBufC16*			iName;
		
		/** Region name for setting item*/
		HBufC16*			iSettingName;

    };

#endif      // FMRADIOREGION_H   
            
// End of File
