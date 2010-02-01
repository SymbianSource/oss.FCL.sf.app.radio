/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioFrequencyNumber
*
*/


#ifndef CFMRADIOFREQUENCYNUMBER_H
#define CFMRADIOFREQUENCYNUMBER_H

#include <eikmfne.h>

/** Flags */
enum
	{
	EFillWithLeadingZeros			=0x1,
	EPreserveOldWidthBeforeEditing	=0x2,
	ERepresentsYear					=0x4,
	EPublicallySettableFlags		=EFillWithLeadingZeros|EPreserveOldWidthBeforeEditing|ERepresentsYear,
	EIsBeingEditedWithCursor		=0x8,
	EIsUninitialised				=0x10
	};

/**
 * Custom field for frequency number.
 */
class CFMRadioFrequencyNumber : public CEikMfneField
    {

public:  // Methods

    /**
     * Static constructor.
     */
    static CFMRadioFrequencyNumber* NewL( 
        const CFont& aFont, 
        TInt aMinimumValue, 
        TInt aMaximumValue, 
        TInt aInitialValue, 
        TUint32 aFlags );
    
    /**
     * Destructor.
     */
    ~CFMRadioFrequencyNumber();

	/**
     * Sets new value in this number field.
     *
     * @param aValue New value
     * @param aFont Current font
     */
	void SetValue( TInt aValue, const CFont& aFont );

	/**
     * Gets the value in this number field.
     *
     * @return Value
     */
	TInt Value() const;

	/**
     * Sets digit type of this number field.
     *
     * @param aDigitType New digit type
     * @param aFont Current font
     */
	void SetDigitType( TDigitType aDigitType, const CFont& aFont );
	
	/**
     * Gets digit type of this number field.
     *
     * @return aDigitType Digit type
     */
	TDigitType DigitType() const;
    
    // from base class CEikMfneField
    /**
     * From CEikMfneField 
     *
     * @see CEikMfneField::IsValid()
     */
    TBool IsValid() const;

	/**
     * From CEikMfneField 
     * 
     * @see CEikMfneField::IsEditable.
     */
	TBool IsEditable() const;

	/**
     * From CEikMfneField 
     * 
     * @see CEikMfneField::HighlightType.
     */
	THighlightType HighlightType() const;

	/**
     * From CEikMfneField 
     * CEikMfneField::HandleKey( 
     *    const CFont& aFont, 
	 *    const TKeyEvent& aKeyEvent, 
	 *    TBool aInterpretLeftAndRightAsEarEvents, 
	 *    TBool& aDataAltered, 
	 *    TInt& aHighlightIncrement 
     */
	void HandleKey(
	    const CFont& aFont, 
	    const TKeyEvent& aKeyEvent, 
	    TBool aInterpretLeftAndRightAsEarEvents, 
	    TBool& aDataAltered, 
	    TInt& aHighlightIncrement );

	/**
     * From CEikMfneField 
     *
     * @see CEikMfneField::HandleDeHighlight( 
     *     const CFont& aFont, 
     *     CEikonEnv& aEikonEnv, 
     *     TBool& aDataAltered, 
     *     TBool& aError )  
     */
	void HandleDeHighlight(
	    const CFont& aFont, 
	    CEikonEnv& aEikonEnv, 
	    TBool& aDataAltered, 
	    TBool& aError);
    
protected:  // Methods

private: //Methods

	/**
     * C++ default constructor.
     */
    CFMRadioFrequencyNumber( 
        TInt aMinimumValue, 
        TInt aMaximumValue, 
        TUint32 aFlags );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CFont& aFont, TInt aInitialValue );

	/**
     * Gets maximum width of this field in pixels.
     */
    TInt MaximumWidthInPixels( 
        const CFont& aFont, 
        TBool aShrinkToMinimumSize );

	/**
     * Gets input capabilities of this field.
     */
	TCoeInputCapabilities InputCapabilities() const;

	/**
     * Get textual representation of the value in this field.
     */
	const TDesC& Text() const;

	/**
     * Gets numerical representation of the value in this field.
     */
	TInt ValueFromText() const;

private:    // Data
	/** 
	 * Minimum value of this field. 
	 */
    TInt iMinimumValue;
    
	/** 
	 * Maximum value of this field. 
	 */
	TInt iMaximumValue;
	
	/** 
	 * Maximum number of digits value in this field. 
	 */
	TInt iMaxDigits;
	
	/** 
	 * Flags of this field. 
	 */
	TUint32 iFlags;
	
	/** 
	 * Holds the value of this field. 
	 */
	HBufC* iText;
	
	/** 
	 * Digit type of this field. 
	 */
	TDigitType iDigitType;

    };

#endif      // CFMRADIOFREQUENCYNUMBER_H   
            
