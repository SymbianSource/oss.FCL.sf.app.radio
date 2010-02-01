/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file of CFMRadioGlobalConfirmationQuery
*
*/


#ifndef C_CFMRADIOGLOBALCONFIRMATIONQUERY_H
#define C_CFMRADIOGLOBALCONFIRMATIONQUERY_H

#include <e32base.h>

class CAknGlobalConfirmationQuery;

/**
* Observer for global confirmation query event.
*/
class MFMRadioGlobalConfirmationQueryObserver
	{
	public:

    /**
	 * Callback function that is called when global confirmation
	 * query is dismissed.
	 *
	 * @param aSoftKey The softkey used to dismiss the query.
	 */
	virtual void GlobalConfirmationQueryDismissedL(TInt aSoftKey) = 0;
	};


/**
 *  FM Radio global confirmation query.
 *
 *  Must be deleted after used once.
 */
class CFMRadioGlobalConfirmationQuery : public CActive
    {

public:
    
	/**
	 * Static constructor.
	 */
    static CFMRadioGlobalConfirmationQuery* NewL(MFMRadioGlobalConfirmationQueryObserver* aObserver = NULL);
    
    /**
	 * Static constructor.
	 */
    static CFMRadioGlobalConfirmationQuery* NewLC(MFMRadioGlobalConfirmationQueryObserver* aObserver = NULL);

    /**
     * Destructor.
     */
    virtual ~CFMRadioGlobalConfirmationQuery();
    
    /**
     * Shows the query.
     *
     * @param   aText       Query text.
     * @param   aSoftkeys   Query softkeys.
     * @param   aAnimation  Query animation.
     */
    void ShowQueryL(const TDesC& aText, TInt aSoftkeys, TInt aAnimation);

// From base class CActive
protected: 
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    /**
     * C++ default constructor.
     */
    CFMRadioGlobalConfirmationQuery(MFMRadioGlobalConfirmationQueryObserver* aObserver = NULL);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private: // data

    /**
     * Pointer to avkon global confirmation query. Owned!
     */
    CAknGlobalConfirmationQuery*        iAknGlobalConfirmationQuery;
    
    /**
     * Text of global confirmation query. Owned!
     */
    HBufC*                              iText;
                               
    /**
     * Observer for global confirmation query. Not owned!
     */
    MFMRadioGlobalConfirmationQueryObserver* iObserver; 
    };

#endif // C_CFMRADIOGLOBALCONFIRMATIONQUERY_H
