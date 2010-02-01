/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Custom editor for editing radio frequencies.
*
*/


#include <AknUtils.h>
#include <barsread.h>
#include <coemain.h>
#include <e32base.h>
#include <eikfctry.h>

#include "fmradiofrequencyeditor.h"
#include "fmradioengineradiosettings.h"
#include "fmradioappui.h"
#include "fmradiofrequencynumber.h"
#include "fmradio.hrh"
#include "debug.h"

const TInt KNumFields = 3;        // Number of fields.
const TInt KIntField = 0;        // Index of integer field.
const TInt KDecField = 2;        // Index of decimal field.

const TInt KFMRadioAbsoluteMinimumIntFieldValue = 0;
const TInt KFMRadioAbsoluteMinimumDecFieldValue = 0;


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::CFMRadioFrequencyEditor
// ---------------------------------------------------------------------------
//
CFMRadioFrequencyEditor::CFMRadioFrequencyEditor() :
        iMinFreq( 0 ),
        iMaxFreq( 0 ),
        iPreviousField( KErrNotFound ), 
        iTypedInDecField( EFalse ),
        iValidToReport( EFalse ),
        iDecDiv( KFMRadioFreqMultiplier ),
        iReportEditorState ( ETrue )
    {
    }


// ---------------------------------------------------------------------------
// Editor is contructed via this static method by the control framework.
// ---------------------------------------------------------------------------
//
SEikControlInfo CFMRadioFrequencyEditor::StaticCreateCustomEditor( 
        TInt aIdentifier )
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::StaticCreateCustomEditor") ) );
    SEikControlInfo customInfo;
    // zero all variables
    Mem::FillZ( &customInfo, sizeof( SEikControlInfo ) ); 
    if ( aIdentifier == EFMRadioFrequencyEditor )
        {
        customInfo.iControl = new CFMRadioFrequencyEditor;
        }
    return customInfo;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::ConstructL( const TUint32 aFlags )
    {
    // Creates space for the fiels in this MFNE.
    CreateFieldArrayL( KNumFields );

    // RadioSettings values are so widely used in this class, that it is 
    // better to read them into member variables.
    CRadioEngine* radioEngine = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() )->RadioEngine();
    TInt minFreq = 0;
    TInt maxFreq = 0;
    radioEngine->GetFrequencyBandRange( minFreq, maxFreq );
    iMinFreq = minFreq / KFMRadioFreqMultiplier;
    iMaxFreq = maxFreq / KFMRadioFreqMultiplier;
    iStepFreq = radioEngine->FrequencyStepSize();
    // Maximum values of fields.
    TInt maxIntFieldValue( 0 );
    TInt maxIntFreq = iMaxFreq / KFMRadioFreqMultiplier;
    while ( maxIntFieldValue < maxIntFreq )
        {
        maxIntFieldValue = maxIntFieldValue*10 + 9;
        }
    TInt maxDecFieldValue( 0 );
    TInt decimalCounter = radioEngine->DecimalCount();
    while ( decimalCounter-- )
        {
        maxDecFieldValue = maxDecFieldValue*10 + 9;
        // Calculate also the decimal divider for later use.
        iDecDiv = iDecDiv / 10;
        }

    // Integer field. Ownership transfers.
    CFMRadioFrequencyNumber* field = CFMRadioFrequencyNumber::NewL( *Font(), 
        KFMRadioAbsoluteMinimumIntFieldValue, maxIntFieldValue, 
        KFMRadioAbsoluteMinimumIntFieldValue, aFlags );
    field->SetDigitType( AknTextUtils::NumericEditorDigitType(), *Font() );
    AddField( field );

    // Decimal separator field. Ownership transfers.
    HBufC* delim = HBufC::NewLC( 1 );
    TLocale loc;
    delim->Des().Append( loc.DecimalSeparator() );
    AddField( CEikMfneSeparator::NewL( delim ) );
    CleanupStack::Pop(); // delim

    // Decimal field. Ownership transfers.
    field = CFMRadioFrequencyNumber::NewL( *Font(), KFMRadioAbsoluteMinimumDecFieldValue, 
        maxDecFieldValue, KFMRadioAbsoluteMinimumDecFieldValue, 
        aFlags | CEikMfneNumber::EFillWithLeadingZeros );
    field->SetDigitType( AknTextUtils::NumericEditorDigitType(), *Font() );
    AddField( field );
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::~CFMRadioFrequencyEditor
// ---------------------------------------------------------------------------
//
CFMRadioFrequencyEditor::~CFMRadioFrequencyEditor()
    {    
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::SetFrequency
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::SetFrequency( const TUint32 aFreq )
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::SetFrequency (aFreq=%d) "), aFreq ) );
    TUint32 freq = aFreq;
    if ( ( iMinFreq != 0 ) && ( iMaxFreq != 0 ) )
        {
        if ( freq < iMinFreq )
            {
            freq = iMinFreq;
            }
        else if ( freq > iMaxFreq )
            {
            freq = iMaxFreq;
            }
        else {}
        }
    TInt intVal = freq / KFMRadioFreqMultiplier;
    TInt decVal = ( freq % KFMRadioFreqMultiplier ) / iDecDiv;
    static_cast<CFMRadioFrequencyNumber*>( 
        Field( KIntField ) )->SetValue( intVal, *Font() );
    static_cast<CFMRadioFrequencyNumber*>( 
        Field( KDecField ) )->SetValue( decVal, *Font() );
    UpdateMinimumAndMaximum();
    ValidateFields();
    DrawNow();
    }


// ---------------------------------------------------------------------------
// Calculates the frequency from values of the fields.
// ---------------------------------------------------------------------------
//        
TUint32 CFMRadioFrequencyEditor::Frequency() const
    {
    TInt intVal = static_cast<CFMRadioFrequencyNumber*>( 
        Field( KIntField ) )->Value();
    TInt decVal = static_cast<CFMRadioFrequencyNumber*>( 
        Field( KDecField ) )->Value();
    return ( KFMRadioFreqMultiplier * intVal ) + ( iDecDiv * decVal );
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::SetMinimumAndMaximum
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::SetMinimumAndMaximum( 
        const TUint32 aMinimumFrequency, const TUint32 aMaximumFrequency )
    {
    iMinIntValue = aMinimumFrequency / KFMRadioFreqMultiplier;
    iMaxIntValue = aMaximumFrequency / KFMRadioFreqMultiplier;
    iMinDecValue = ( aMinimumFrequency % KFMRadioFreqMultiplier ) / iDecDiv;
    iMaxDecValue = ( aMaximumFrequency % KFMRadioFreqMultiplier ) / iDecDiv;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::GetMinimumAndMaximum
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::GetMinimumAndMaximum( TUint32& aMinimumFrequency, 
        TUint32& aMaximumFrequency ) const
    {
    aMinimumFrequency = ( KFMRadioFreqMultiplier * iMinIntValue ) + 
        ( iDecDiv*iMinDecValue );
    aMaximumFrequency = ( KFMRadioFreqMultiplier * iMaxIntValue ) + 
        ( iDecDiv * iMaxDecValue );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CFMRadioFrequencyEditor::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::ConstructFromResourceL( 
        TResourceReader& aResourceReader )
    {
    TUint32 flags = aResourceReader.ReadUint8();
    ConstructL( EAknEditorFlagDeliverVirtualKeyEventsToApplication );
    }


// ---------------------------------------------------------------------------
// From class CEikMfne.
// Every key press may cause some recalculations and/or validations in 
// the fields.
// ---------------------------------------------------------------------------
//
TKeyResponse CFMRadioFrequencyEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::OfferKeyEventL ") ) );
    iPreviousField = CurrentField();

    TKeyResponse resp( EKeyWasNotConsumed );
    
    CEikMfneField* currField = Field( CurrentField() );

    // Check only once per key press.
    if ( aType == EEventKey )
        {
        TBool dataAltered = EFalse;
        TBool error = EFalse;
        TInt changeField = 0;
        TInt newCurrentField = CurrentField();
        
        switch ( aKeyEvent.iCode )
            {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                {
                currField->HandleKey( *Font(), aKeyEvent, EFalse, dataAltered, 
                    changeField );

                // If after number key press we are still on decimal field 
                // (and were before).
                if ( ( !changeField ) && ( CurrentField() == KDecField ) )
                    {
                    // This can mean only that user has typed one number in 
                    // the decimal field and system is waiting for the other.
                    iTypedInDecField = ETrue;
                    }
                else
                    {
                    iTypedInDecField = EFalse;
                    }
                // If after number key press system moved to another field 
                // (previous field was filled with numbers). 
                if ( changeField )
                    {
                    if ( CurrentField() == KIntField )
                        {
                        // First check if the changed value is within limits.
                        error = ValidateIntField();    
                        // Calculate new minimums and maximums.
                        UpdateMinimumAndMaximum();    
                        // Now have to check also whether the other field is 
                        // within new limits.
                        ValidateDecField();            
                        }
                    else if ( CurrentField() == KDecField )
                        {
                        error = ValidateDecField();
                        }
                    else {}
                    }
                break;
                }
            case EKeyBackspace:        
                {
                // Pressing "c"-key in a field will set the field in 
                // "typed" state.
                currField->HandleKey( *Font(), aKeyEvent, EFalse, dataAltered, 
                    changeField );
                if ( CurrentField() == KDecField )
                    {
                    iTypedInDecField = ETrue;
                    }
                break;
                }
            case EKeyLeftArrow: // fall-through intended here
            case EKeyRightArrow:
                {
                currField->HandleKey( *Font(), aKeyEvent, EFalse, dataAltered, 
                    changeField );
                FillDecimalField();
                ValidateFields();
                // Minimums and maximums may have to be changed.
                UpdateMinimumAndMaximum();    
                // Validate according to new minimums and maximums.
                ValidateFields();            
                break;
                }
            default:
                {
                break;
                }
            }

        if ( changeField )
            {
            if ( CurrentField() == KIntField )
                {
                newCurrentField = KDecField;
                }
            else
                {
                newCurrentField = KIntField;
                }
            }
        TBuf<3> fieldText;
        TInt fieldVal = 0;
        TRAPD( err, fieldVal = static_cast<CFMRadioFrequencyNumber*>( currField )->Value() )
        if ( !err )
            {
            fieldText.Num( static_cast<TInt64>( fieldVal ) );
            }
        CEikMfne::HandleInteraction( changeField, newCurrentField,
                                     Font()->TextWidthInPixels( fieldText ),
                                     currField->HighlightType(),
                                     dataAltered,
                                     error );

        DrawAndReportL( EFalse );
        }

    return resp;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::FillDecimalField
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::FillDecimalField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::FillDecimalField") ) );
    if ( iTypedInDecField )
        {    
        CFMRadioFrequencyNumber* field = 
            static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
        // Multiply value by iDecDiv.
        field->SetValue( iDecDiv*field->Value(), *Font() );    
        iTypedInDecField = EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::CanLeaveEditorL
// ---------------------------------------------------------------------------
//
TBool CFMRadioFrequencyEditor::CanLeaveEditorL()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::CanLeaveEditorL") ) );
    TBool ret = ETrue;

    FillDecimalField();

    if ( CurrentField() == KIntField )
        {
        // First check if the changed value is within limits.
        ret = !ValidateIntField();    
        // Calculate new minimums and maximums.
        UpdateMinimumAndMaximum();    
        // Now have to check also whether the other field is within new 
        // limits.
        ValidateDecField();            
        }
    else if ( CurrentField() == KDecField )
        {
        ret = !ValidateDecField();
        }
    else {}

    PrepareForFocusLossL();

    // Draw immediately, otherwise a wrong number is displayed in decimal 
    // field (for example: 03 instead of 30)
    DrawAndReportL( ETrue ); 
                           
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::IncrementCurrentField
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::IncrementCurrentField()
	{
	CEikMfneField* currField = Field( CurrentField() );
	 // If user is not in the middle of typing numbers.
    if ( currField->HighlightType() == 
            CEikMfneField::EInverseVideo )
        {
        if ( CurrentField() == KIntField )
            {
            IncrementIntField();
            }
        else if ( CurrentField() == KDecField )
            {
            IncrementDecField();
            }
        else {}
        // Minimums and maximums may have to be changed.
        UpdateMinimumAndMaximum();    
        // Validate according to new minimums and maximums.
        ValidateFields();            
        }
        
    TRAP_IGNORE( DrawAndReportL( EFalse ) );
	}
	
// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::DecrementCurrentField
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::DecrementCurrentField()
	{
	CEikMfneField* currField = Field( CurrentField() );
	 // If user is not in the middle of typing numbers.
    if ( currField->HighlightType() == 
            CEikMfneField::EInverseVideo )
        {
        if ( CurrentField() == KIntField )
            {
            DecrementIntField();
            }
        else if ( CurrentField() == KDecField )
            {
            DecrementDecField();
            }
        else {}
        // Minimums and maximums may have to be changed.
        UpdateMinimumAndMaximum();    
        // Validate according to new minimums and maximums.
        ValidateFields();            
        }
    TRAP_IGNORE( DrawAndReportL( EFalse ) );
	}

// ---------------------------------------------------------------------------
// Field decrement step is 1.
// If field value is already at maximum, it set to minimum.
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::IncrementIntField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::IncrementIntField") ) );
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KIntField ) );
    CFMRadioFrequencyNumber* decField = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
        
    if ( field->Value() < iMaxIntValue )
        {
        field->SetValue( field->Value() + 1, *Font() );
        }
    else if ( field->Value() == iMaxIntValue && decField->Value() == iMaxDecValue)
        {
        field->SetValue( iMinIntValue, *Font() );
        }
    else 
        {
        decField->SetValue( iMaxDecValue, *Font() );        
        }
    }
    
    
// ---------------------------------------------------------------------------
// Field decrement step is 1.
// If field value is already at minimum, it set to maximum.
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::DecrementIntField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::DecrementIntField") ) );
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KIntField ) );
    CFMRadioFrequencyNumber* decField = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
    
    if ( field->Value() > iMinIntValue )
        {
        field->SetValue( field->Value() - 1, *Font() );
        }
    else if ( field->Value() == iMinIntValue && decField->Value() == iMinDecValue)
        {
        field->SetValue( iMaxIntValue, *Font() );
        }
    else 
        {
        decField->SetValue( iMinDecValue, *Font() );
        }
    }
    
    
// ---------------------------------------------------------------------------
// Field inrement step is defined in RadioSettings.
// If field value is already at maximum, integer field
// has to be incremented and this field set to minimum.
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::IncrementDecField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::IncrementDecField") ) );
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
    if ( field->Value() < iMaxDecValue )
        {
        field->SetValue( field->Value() + iStepFreq/iDecDiv, *Font() );
        }
    else if ( field->Value() == iMaxDecValue )
        {
        IncrementIntField();
        // This is done because min of this field may change when integer 
        // field is set to it's minimum.
        UpdateMinimumAndMaximum();    
        field->SetValue( iMinDecValue, *Font() );
        }
    else {}
    }
    
    
// ---------------------------------------------------------------------------
// Field decrement step is defined in RadioSettings.
// If field value is already at minimum, integer field
// has to be decremented and this field set to maximum.
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::DecrementDecField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::DecrementDecField") ) );
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
    if ( field->Value() > iMinDecValue )
        {
        field->SetValue( field->Value() - iStepFreq/iDecDiv, *Font() );
        }
    else if ( field->Value() == iMinDecValue )
        {
        DecrementIntField();
        // This is done because max of this field may change when integer 
        // field is set to it's maximum.
        UpdateMinimumAndMaximum();    
        field->SetValue( iMaxDecValue, *Font() );
        }
    else {}
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::ValidateFields
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::ValidateFields()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::ValidateFields") ) );
    ValidateIntField();
    ValidateDecField();
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::ValidateIntField
// ---------------------------------------------------------------------------
//
TBool CFMRadioFrequencyEditor::ValidateIntField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::ValidateIntField") ) );
    TBool ret = EFalse;
    
    // Check if integer field value is within limits.
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KIntField ) );
    if ( field->Value() < iMinIntValue )
        {
        field->SetValue( iMinIntValue, *Font() );
        ret = ETrue;
        }
    else if ( field->Value() > iMaxIntValue )
        {
        field->SetValue( iMaxIntValue, *Font() );
        ret = ETrue;
        }
    else {}

    return ret;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::ValidateDecField
// ---------------------------------------------------------------------------
//
TBool CFMRadioFrequencyEditor::ValidateDecField()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::ValidateDecField") ) );
    TBool ret = EFalse;

    // Check if decimal field value is within limits.
    CFMRadioFrequencyNumber* field = 
        static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
    if ( field->Value() < iMinDecValue )
        {
        field->SetValue( iMinDecValue, *Font() );
        ret = ETrue;
        }
    else if ( field->Value() > iMaxDecValue )
        {
        field->SetValue( iMaxDecValue, *Font() );
        ret = ETrue;
        }
    else
        {
        // Check if decimal field value is within a step.
        TInt remainder = static_cast<TUint32>( 
            ( field->Value()-iMinDecValue ) ) % ( iStepFreq/iDecDiv );
        if ( remainder != 0 )
            {
            field->SetValue( field->Value()-remainder, *Font() );
            ret = ETrue;
            }
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::UpdateMinimumAndMaximum
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::UpdateMinimumAndMaximum()
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyEditor::UpdateMinimumAndMaximum") ) );
    TInt minIntValue( 0 );
    TInt maxIntValue( 0 );
    TInt minDecValue( 0 );
    TInt maxDecValue( 0 );
    
    minIntValue = iMinFreq / KFMRadioFreqMultiplier;
    maxIntValue = iMaxFreq / KFMRadioFreqMultiplier;
        
    if ( ( Frequency() / KFMRadioFreqMultiplier > iMinFreq / KFMRadioFreqMultiplier ) && 
            ( Frequency() / KFMRadioFreqMultiplier < iMaxFreq / KFMRadioFreqMultiplier ) )
        {    
        // Remainder of the gap from min freq to next int value.
        minDecValue = ( ( ( KFMRadioFreqMultiplier * ( minIntValue + 1 ) ) - iMinFreq ) 
            % iStepFreq ) / iDecDiv;
        // Same kind of thing vice versa.
        maxDecValue = ( ( ( iMinFreq - ( KFMRadioFreqMultiplier * ( minIntValue ) ) ) 
            % iStepFreq ) / iDecDiv ) + KFMRadioFreqMultiplier / iDecDiv 
            - ( iStepFreq / iDecDiv );    
        }
    else if ( Frequency() / KFMRadioFreqMultiplier == iMinFreq / KFMRadioFreqMultiplier )
        {
        minDecValue = ( iMinFreq % KFMRadioFreqMultiplier ) / iDecDiv;
        // Same kind of thing vice versa.
        maxDecValue = ( ( ( iMinFreq - ( KFMRadioFreqMultiplier * ( minIntValue ) ) ) 
            % iStepFreq ) / iDecDiv ) + KFMRadioFreqMultiplier / iDecDiv 
            - ( iStepFreq / iDecDiv );
        }
    else if ( Frequency() / KFMRadioFreqMultiplier == iMaxFreq / KFMRadioFreqMultiplier )
        {
        // Remainder of the gap from min freq to next int value.
        minDecValue = ( ( ( KFMRadioFreqMultiplier * ( minIntValue + 1 ) ) - iMinFreq ) 
            % iStepFreq ) / iDecDiv;    
        maxDecValue = ( iMaxFreq % KFMRadioFreqMultiplier ) / iDecDiv;
        }
    else {}

    SetMinimumAndMaximum( 
        ( KFMRadioFreqMultiplier * minIntValue ) + ( iDecDiv * minDecValue ), 
        ( KFMRadioFreqMultiplier * maxIntValue ) + ( iDecDiv * maxDecValue ) );
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::IsValid
// ---------------------------------------------------------------------------
//
TBool CFMRadioFrequencyEditor::IsValid() const
    {
    return ( Field( CurrentField() )->HighlightType() == 
        CEikMfneField::EInverseVideo );
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::IsValidToReport
// ---------------------------------------------------------------------------
//
TBool CFMRadioFrequencyEditor::IsValidToReport() const
    {
    return iValidToReport;
    }


// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::DrawAndReportL
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::DrawAndReportL( TBool /*aForceDrawNow*/ )
    {
    //Switched to always do DrawNow() - DrawDeferred() is too slow for rapid
    //frequency changing (long press accelerates after a while)
//    if ( !aForceDrawNow && ( DrawableWindow() && !IsBackedUp() ) )
//        {
//        // Flicker-free redraw.
//        DrawDeferred();
//        }
//    else
//        {
//        DrawNow();
//        }	

    DrawNow();
    

	if ( iReportEditorState )
		{
		iValidToReport = ETrue;		
		}
	else
		{
		iValidToReport = EFalse;	
		}		
    // Report back finally to main container.
    ReportEventL( MCoeControlObserver::EEventStateChanged );    
    iValidToReport = EFalse;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CFMRadioFrequencyEditor::FocusChanged
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    if ( Field( CurrentField() )->HighlightType() == 
            CEikMfneField::EInverseVideo )
        {
        CFMRadioFrequencyNumber* field = 
            static_cast<CFMRadioFrequencyNumber*>( Field( KIntField ) );
        field->SetDigitType( AknTextUtils::NumericEditorDigitType(), 
            *Font() );
        field = NULL;
        field = static_cast<CFMRadioFrequencyNumber*>( Field( KDecField ) );
        field->SetDigitType( AknTextUtils::NumericEditorDigitType(), 
            *Font() );
        }

    if ( DrawableWindow() && !IsBackedUp() )
        {
        // Flicker-free redraw.
        reinterpret_cast<RWindow*>( DrawableWindow() )->Invalidate();
        }
    else
        {
        DrawNow();
        }
    }

void CFMRadioFrequencyEditor::SetEditorReportState( const TBool aReport )
	{
	iReportEditorState = aReport;
	}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// CFMRadioFrequencyEditor::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        TBool dataAltered = EFalse;
        TInt newCurrentField = CurrentField();
        TInt error = KErrNone;
    
        CEikMfneField* currField = Field( CurrentField() );
        TInt fieldWidth = iEditorFrameRect.Width() / 2;
        TRect fieldRect;
        
        if ( CurrentField() == KIntField )
            {
            newCurrentField = KDecField;
            TPoint fieldPos( iEditorFrameRect.iTl );
            fieldPos.iX += iEditorFrameRect.Width() - fieldWidth;
            fieldRect.SetRect( fieldPos, TSize( fieldWidth , iEditorFrameRect.Height() ) );
            }
        else
            {
            newCurrentField = KIntField;
            fieldRect.SetRect( iEditorFrameRect.iTl, TSize( fieldWidth , iEditorFrameRect.Height() ) );
            }

        if ( fieldRect.Contains( aPointerEvent.iPosition ) )
            {
            FillDecimalField();
            ValidateFields();
            // Minimums and maximums may have to be changed.
            UpdateMinimumAndMaximum();
            // Validate according to new minimums and maximums.
            ValidateFields();

             TBuf<3> fieldText;
             TInt fieldVal = 0;
             TRAPD( err, fieldVal = static_cast<CFMRadioFrequencyNumber*>( currField )->Value() )
             if ( !err )
                 {
                 fieldText.Num( static_cast<TInt64>( fieldVal ) );
                 }
             CEikMfne::HandleInteraction( ETrue,
                                          newCurrentField, 
                                          Font()->TextWidthInPixels( fieldText ), 
                                          currField->HighlightType(),
                                          dataAltered,
                                          error );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioFrequencyEditor::SetEditorFrameRect
// ---------------------------------------------------------------------------
//
void CFMRadioFrequencyEditor::SetEditorFrameRect( const TRect& aRect )
    {
    iEditorFrameRect = aRect;
    }

// End of file
