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
* Description:  Header file for CFMRadioFrequencyQueryDialog
*
*/


#ifndef FMRADIOFREQUENCYQUERYDIALOG_H
#define FMRADIOFREQUENCYQUERYDIALOG_H

#include <AknQueryDialog.h>

/**
 * Custom query dialog for frequency.
 */
class CFMRadioFrequencyQueryDialog : public CAknQueryDialog
    {

public:  // Methods

    /**
     * Static constructor.
     */
    static CFMRadioFrequencyQueryDialog* NewL( TUint32& aFreq, 
        const TTone& aTone = ENoTone );
    
    /**
     * Destructor.
     */
    ~CFMRadioFrequencyQueryDialog();

	/**
     * Returns the frequency currently set in this dialog.
     *
     * @return ?description
     */
	TUint32 Frequency() const;

    // from base class MAknQueryControlObserver
	/**
     * From MAknQueryControlObserver 
     *
     * @see MAknQueryControlObserver::HandleQueryEditorStateEventL
	 */
	TBool HandleQueryEditorStateEventL( 
	    CAknQueryControl* aQueryControl, 
	    TQueryControlEvent aEventType, 
	    TQueryValidationStatus aStatus );
    
protected:  // Methods
   
    // from base class CAknQueryDialog
    /**
     * From CAknQueryDialog 
     * 
     * @see CAknQueryDialog::QueryControl() const
     */
    CAknQueryControl* QueryControl() const;

	/**
	 * From CAknQueryDialog 
	 * 
	 * @see CAknQueryDialog::UpdateLeftSoftKeyL()
	 */
	void UpdateLeftSoftKeyL();

	/**
     * From CAknQueryDialog OkToExitL
     */
	TBool OkToExitL(TInt aButtonId);

    // from base class CEikDialog
	/**
     * From CEikDialog
     *
     * @see CEikDialog::PreLayoutDynInitL()
     */
	void PreLayoutDynInitL();

private: //Methods

	/**
     * C++ default constructor.
     */
    CFMRadioFrequencyQueryDialog( TUint32& aFreq, const TTone& aTone = ENoTone );


private:

	/** 
	 * Current frequency 
	 */
    TUint32& iFreq;

    };

#endif      // FMRADIOFREQUENCYQUERYDIALOG_H   

