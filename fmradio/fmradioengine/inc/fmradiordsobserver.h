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
* Description:  Observer interface for RDS events
*
*/


#ifndef MFMRADIORDSOBSERVER_H
#define MFMRADIORDSOBSERVER_H

class MFMRadioRdsObserver
    {
public:

    /**
     * Programme Service data available
     *
     * @param aProgramService The new Programme Service data
     */  
    virtual void RdsDataProgrammeService( const TDesC& aProgramService ) = 0;
    
    /**
     * Reports if the programme service name is detected as dynamically changing or not
     *
     * @param aStatic ETrue if the name is static, EFalse if it is dynamic
     */  
    virtual void RdsDataPsNameIsStatic( TBool aStatic ) = 0;
    
    /**
     * Radio text data available
     *
     * @param aRadioText The new Radio text data
     */  
    virtual void RdsDataRadioText( const TDesC& aRadioText ) = 0;

    /**
     * Radio text plus data available
     *
     * @param aRTPlusClass The type of RT+ text
     * @param aRadioTextPlus The new Radio text data
     */  
    virtual void RdsDataRadioTextPlus( const TInt aRTPlusClass, const TDesC& aRadioTextPlus ) = 0;
       
    /**
     * Notifies of the beginning of Alternate Frequency search
     */
    virtual void RdsAfSearchBegin() = 0;
    
    /**
     * Notifies of the completed Alternate Frequency search
     *
     * @param aFrequency The new frequency
     * @param aError A standard system error code
     */
    virtual void RdsAfSearchEnd( TUint32 aFrequency, TInt aError ) = 0;

    /**
     * Notifies of the changed Alternate Frequency search state
     *
     * @param aEnabled ETrue if AF search was enabled, otherwise EFalse
     */
    virtual void RdsAfSearchStateChange( TBool aEnabled ) = 0;

    /**
     * Notifies of the status of RDS signal
     * 
     * @param aAvailable is RDS available
     */
    virtual void RdsAvailable( TBool aAvailable ) = 0;
    };

#endif // MFMRADIORDSOBSERVER_H
