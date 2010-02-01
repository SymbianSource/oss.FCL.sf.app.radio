/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioFrequencyEditor
*
*/


#ifndef CFMRADIOFREQUENCYEDITOR_H
#define CFMRADIOFREQUENCYEDITOR_H

#include <eikmfne.h>

struct SEikControlInfo;

/**
 * Custom editor for editing radio frequencies.
 */
class CFMRadioFrequencyEditor : public CEikMfne
    {
    
public:  // Methods
   
    /**
     * C++ default constructor.
     */
    CFMRadioFrequencyEditor();

	/**
     * Symbian 2nd phase constructor.
     */
    void ConstructL(const TUint32 aFlags = 0);
    
    /**
     * Destructor.
     */
    ~CFMRadioFrequencyEditor();

	/**
     * This editor is constructed by the system using this static method.
     */
	static SEikControlInfo StaticCreateCustomEditor(TInt aIdentifier);

    /**
     * Sets the frequency of this editor.
     *
     * @param aFreq New frequency.
     */
    void SetFrequency(const TUint32 aFreq);

	/**
     * Returns the frequency of this editor.
     *
     * @return Frequency of this editor.
     */
    TUint32 Frequency() const;

	/**
     * Sets minimum and maximum frequencies of this editor.
     *
     * @param aMinimumFrequency Minimum frequency.
	 * @param aMaximumFrequency Maximum frequency.
     */
    void SetMinimumAndMaximum(const TUint32 aMinimumFrequency, 
        const TUint32 aMaximumFrequency);

	/**
     * Gets minimum and maximum frequencies of this editor.
     *
     * @param aMinimumFrequency Contains the minimum frequency when returned.
	 * @param aMaximumFrequency Contains the maximum frequency when returned.
     */
    void GetMinimumAndMaximum(TUint32& aMinimumFrequency, 
        TUint32& aMaximumFrequency) const;

	/**
	 * Returns validity of this editor.
	 * @return ETrue, if the values of the fields of this editor are valid. 
	 * EFalse otherwise.
	 */
	TBool IsValid() const;

	/**
	 * Returns the report state of this editor.
	 * @return ETrue, if the editor is valid to report of a state change. 
	 * EFalse otherwise.
	 */
	TBool IsValidToReport() const;

	/**
	 * Fills decimal field with zero decimal, if field is left in middle of 
	 * typing.
	 */
	void FillDecimalField();

	/**
	 * Handle possible editor leave.
	 *
	 * @return ETrue, if editor can be left.
	 */
	TBool CanLeaveEditorL();

    // from base class CCoeControl
    /**
     * From CCoeControl 
     * @see CCoeControl::ConstructFromResourceL
     */
    void ConstructFromResourceL( TResourceReader& aResourceReader );

    // from base class CEikMfne
	/**
	 * From CEikMfne OfferKeyEventL.	
	 * @see CEikMfne::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
	 */
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	/**
	 * From CEikMfne HandlePointerEventL.	
	 * @see CEikMfne::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	 */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	/**
     * Updates minimum and maximum values of the fields.
     */
	void UpdateMinimumAndMaximum();
	
	/**
     * Increments the current field.
     */
	void IncrementCurrentField();

	/**
     * Decrements the current field.
     */
	void DecrementCurrentField();
	
	void SetEditorReportState( TBool aReport );
	
	/**
	 * Redraws editor and reports of state change.
	 *
	 * @param aForceDrawNow forces immediate draw
	 */
	void DrawAndReportL( TBool aForceDrawNow );

    /*
     * Set editor frame rect
     * @param aRect editor frame rect 
     */
    void SetEditorFrameRect( const TRect& aRect );

protected:  // Methods

    // from base class CCoeControl
	/**
     * From CCoeControl 
     *
     * @see CCoeControl::FocusChanged( TDrawNow aDrawNow )
     */
	void FocusChanged( TDrawNow aDrawNow );

private: //Methods

	/**
	 * Increments the integer field.
	 */
	void IncrementIntField();
	
	/**
	 * Decrements the integer field.
	 */
	void DecrementIntField();
	
	/**
	 * Increments the decimal field.
	 */
	void IncrementDecField();
	
	/**
	 * Decrements the decimal field.
	 */
	void DecrementDecField();

	/**
     * Validates the values of the both fields.
	 * Calls ValidateIntField and ValidateDecField,
	 * but doesn't return anything.
     */
	void ValidateFields();

	/**
     * Validates the integer field. If field was changed, returns ETrue.
     */
	TBool ValidateIntField();

	/**
     * Validates the decimal field. If field was changed, returns ETrue.
     */
	TBool ValidateDecField();


	


private:    // Data
	/** 
	 * Minimum frequency got from RadioSettings. 
	 */
    TUint32	iMinFreq;
    
	/** 
	 * Maximum frequency got from RadioSettings. 
	 */
    TUint32 iMaxFreq;
    
	/** 
	 * Step frequency got from RadioSettings. 
	 */
    TUint32 iStepFreq;
    
	/** 
	 * Current minimum value of integer field. 
	 */
	TInt iMinIntValue;
	
	/** 
	 * Current maximum value of integer field. 
	 */
	TInt iMaxIntValue;
	
	/** 
	 * Current minimum value of decimal field. 
	 */ 
	TInt iMinDecValue;
	
	/** 
	 * Current maximum value of decimal field. 
	 */ 
	TInt iMaxDecValue;
	
	/** 
	 * The field at the beginning of keypress. 
	 */
	TInt iPreviousField;
	
	/** 
	 * ETrue, when user is in the middle of typing numbers in decimal 
	 * field. 
	 */
	TBool iTypedInDecField;
	
	/** 
	 * Editor is valid to report it's state (after internal validations). 
	 */
	TBool iValidToReport;
	
	/** 
	 * Decimal field divider that is based on decimal count. 
	 */
	TUint32 iDecDiv;
	/**
	 * flag to set editor report state
	 */
	TBool iReportEditorState;
	
	TInt iIncrements;
	
	/*
	 * Editor frame rect
	 */
	TRect iEditorFrameRect;

    };

#endif      // CFMRADIOFREQUENCYEDITOR_H   
