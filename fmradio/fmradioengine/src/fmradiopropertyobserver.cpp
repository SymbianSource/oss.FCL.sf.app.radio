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
* Description: Observer for FMRadio property values  
*
*/


#include "fmradiopropertyobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFMRadioPropertyObserver::CFMRadioPropertyObserver(MFMRadioPropertyChangeObserver& aObserver, const TUid& aCategory, const TUint aKey, const TFMRadioPropertyType aPropertyType)
    : CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFMRadioPropertyObserver::ConstructL()
	{
    switch (iPropertyType)
        {
        case EFMRadioPropertyInt:
	        {
	        break;
	        }
	    case EFMRadioPropertyByteArray:
	        {
	        iValueByteArray = HBufC8::NewL( RProperty::KMaxPropertySize );
	        break;
	        }
	    case EFMRadioPropertyText:
	        {
	        // Max size in bytes, length is size / 2
	        iValueText = HBufC::NewL( RProperty::KMaxPropertySize / 2 );
	        break;
	        }
	    default:
	        {
	        break;
	        }
        }

    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFMRadioPropertyObserver* CFMRadioPropertyObserver::NewL( MFMRadioPropertyChangeObserver& aObserver,
                                                          const TUid& aCategory,
                                                          const TUint aKey,
                                                          const TFMRadioPropertyType aPropertyType )
    {
    CFMRadioPropertyObserver* self = NewLC( aObserver,
                                            aCategory,
                                            aKey,
                                            aPropertyType );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFMRadioPropertyObserver* CFMRadioPropertyObserver::NewLC( MFMRadioPropertyChangeObserver& aObserver,
														  const TUid& aCategory,
														  const TUint aKey,
														  const TFMRadioPropertyType aPropertyType )
	{
    CFMRadioPropertyObserver* self = new( ELeave )CFMRadioPropertyObserver( aObserver,
    																   		aCategory,
    																   		aKey,
    																   		aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CFMRadioPropertyObserver::~CFMRadioPropertyObserver()
	{
	Cancel();
    iProperty.Close();
    delete iValueByteArray;
    delete iValueText;
	}

// ---------------------------------------------------------------------------
// Subscribes to a property and reads the value, if not already active.
// ---------------------------------------------------------------------------
//
void CFMRadioPropertyObserver::ActivateL()
	{
    if ( !IsActive() )
        {
        RunL();
        }
	}

// -----------------------------------------------------------------------------
// CFMRadioPropertyObserver::RunL
// -----------------------------------------------------------------------------
//
void CFMRadioPropertyObserver::RunL()
	{	
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt err(KErrNone);
    
    switch (iPropertyType)
        {
        case EFMRadioPropertyInt:
	        {
	        err = iProperty.Get( iValueInt );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValueInt );
             	}
	        break;
	        }
		case EFMRadioPropertyByteArray:
	        {
	        TPtr8 ptr8( iValueByteArray->Des() );
	        err = iProperty.Get( ptr8 );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueByteArray );
             	}
	        break;		        
	        }
		case EFMRadioPropertyText:
	        {
	        TPtr ptr( iValueText->Des() );
	        err = iProperty.Get( ptr );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueText );
             	}
	        break;			        
	        }
	        
	    default:
	        {
	        break;
	        }
        }
    
    if (err)
    	{
        iObserver.HandlePropertyChangeErrorL(iCategory, iKey, err);
     	}
	}

// -----------------------------------------------------------------------------
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CFMRadioPropertyObserver::DoCancel()
	{
    iProperty.Cancel();
	}

// -----------------------------------------------------------------------------
// Getter for integer value
// -----------------------------------------------------------------------------
//	
TInt CFMRadioPropertyObserver::ValueInt( TBool aUpdate )
	{
	if( aUpdate )
	    {
	    iProperty.Get( iValueInt );
	    }
	return iValueInt;
	}
	
// -----------------------------------------------------------------------------
// Getter for byte array value
// -----------------------------------------------------------------------------
//	
const TDesC8& CFMRadioPropertyObserver::ValueDes8( TBool aUpdate )
	{
   if( aUpdate )
        {
    	TPtr8 ptr8( iValueByteArray->Des() );
    	iProperty.Get( ptr8 );
        }
	return *iValueByteArray;
	}
	
// -----------------------------------------------------------------------------
// Getter for text value
// -----------------------------------------------------------------------------
//	
const TDesC& CFMRadioPropertyObserver::ValueDes( TBool aUpdate )
	{
   if( aUpdate )
        {
    	TPtr ptr( iValueText->Des() );
    	iProperty.Get( ptr );
        }
	return *iValueText;
	}

