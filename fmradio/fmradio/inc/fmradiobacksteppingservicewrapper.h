/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the class CFMRadioBackSteppingServiceWrapper
*
*/

#ifndef CFMRADIOBACKSTEPPINGSERVICEWRAPPER_H
#define CFMRADIOBACKSTEPPINGSERVICEWRAPPER_H

#include <coeview.h>
#include <e32base.h>
#include <vwsdef.h>

class MLiwInterface;
class CLiwGenericParamList;
class CLiwServiceHandler;

/**
 *  Back Stepping (BS) Service wrapper.
 */
class CFMRadioBackSteppingServiceWrapper : public CBase, public MCoeViewActivationObserver
    {

public:

    /**
     * Static constructor.
     * The object can be registered as view activation observer 
     * or view activations can be notified explicitly by HandleViewActivationEventL.
     * 
     * @param aUid Application UID.
     */
    static CFMRadioBackSteppingServiceWrapper* NewL( TUid aUid );

    /**
     * Destructor.
     */
     ~CFMRadioBackSteppingServiceWrapper();

    /**
     * Forwards view activation event to BS Service.
     *
     * @param   aViewId View id of the view where back is called.
     * @param   aEnter Indicates if it is entry or exit activation.
     */
    void HandleViewActivationEventL( const TVwsViewId aViewId, TBool aEnter );

    /**
     * Forwards back command to BS Service.
     *
     * @param   aViewId View id of the view where back is called.
     * @return  Returns Etrue if BS Service consumed the command, otherwise returns EFalse.
     */
    TBool HandleBackCommandL( const TVwsViewId aViewId );

private:

    /**
     * C++ default constructor.
     */
    CFMRadioBackSteppingServiceWrapper();

    /**
     * 2nd phase constructor
     *
     * @param aUid Application UID.
     */
    void ConstructL( TUid aUid );

    /**
     * Initializes BS Service.
     *
     * @param aUid Application UID.
     */
    void InitializeL( TUid aUid );

    /**
     * Handles the result of a LIW command
     *
     * @return Returns ETrue if LIW command executed, otherwise returns EFalse.
     */
    TBool HandleResultL();
    
    // from base class MCoeViewActivationObserver
    void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
                               const TVwsViewId& aViewIdToBeDeactivated );

private: // data

    /**
     * AIW Service Handler.
     * Owned.
     */
    CLiwServiceHandler* iServiceHandler;

    /**
     * BS Service interface returned by LIW.
     * Owned.
     */
    MLiwInterface* iBsInterface;

    /**
     * In param list.
     * Not owned.
     */
    CLiwGenericParamList* iInParamList;

    /**
     * Out param list.
     * Not owned.
     */
    CLiwGenericParamList* iOutParamList;
    };

#endif // CFMRADIOBACKSTEPPINGSERVICEWRAPPER_H
