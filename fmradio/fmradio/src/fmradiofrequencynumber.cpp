/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Frequency number handling for manual tuning dialog
*
*/


// INCLUDE FILES

#include	<aknappui.h>
#include	<aknsoundsystem.h>
#include	<AknTextDecorationMetrics.h>
#include	<numberconversion.h>
#include	<EIKCTL.rsg>

#include    "fmradiofrequencynumber.h"
#include 	"debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CFMRadioFrequencyNumber::CFMRadioFrequencyNumber(TInt aMinimumValue, TInt aMaximumValue, TUint32 aFlags)
	: iMinimumValue(aMinimumValue), iMaximumValue(aMaximumValue), iFlags(aFlags)
	{
	// Get maximum number of digits.
	TBuf<16> maxText;
	maxText.AppendNum(static_cast<TInt64>(aMaximumValue));
	iMaxDigits = maxText.Length();
	}

// EPOC default constructor can leave.
void CFMRadioFrequencyNumber::ConstructL(const CFont& aFont, TInt aInitialValue)
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::ConstructL") ) );
	// Init text field containing the value.
	iText = HBufC::NewL(iMaxDigits);
	SetValue(aInitialValue, aFont);
    }

// Two-phased constructor.
CFMRadioFrequencyNumber* CFMRadioFrequencyNumber::NewL(const CFont& aFont, TInt aMinimumValue, TInt aMaximumValue, TInt aInitialValue, TUint32 aFlags)
    {
    FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::NewL") ) );
    CFMRadioFrequencyNumber* self = new (ELeave) CFMRadioFrequencyNumber(aMinimumValue, aMaximumValue, aFlags);
    CleanupStack::PushL(self);
    self->ConstructL(aFont, aInitialValue);
    CleanupStack::Pop();
    return self;
    }

    
// Destructor
CFMRadioFrequencyNumber::~CFMRadioFrequencyNumber()
    {
    delete iText;
    }

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::MaximumWidthInPixels
// TInt addedWith is calculated same way than in TInt CEikMfneField::AdditionalWidthForHighlights, 
// which is public but not exported method.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyNumber::MaximumWidthInPixels(const CFont& aFont, TBool /*aShrinkToMinimumSize*/)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::MaximumWidthInPixels") ) );
	TInt addedWidth = 0;
    if ( IsEditable() )
        {
        TInt rightHighlight = 0;
        TAknTextDecorationMetrics metrics( &aFont );
        // Width is increased only by leftHighlight.  This is a compromise in that glyphs already have 
        // spacing within them to achieve character spacing. This spacing is generally (for numbers) on the right. 
        metrics.GetLeftAndRightMargins( addedWidth, rightHighlight ); // rightHighlight is not used
        }

	return (iMaxDigits * TFindWidthOfWidestDigitType(iDigitType).MaximumWidthInPixels(aFont)) + addedWidth;
	}


// ---------------------------------------------------------
// CFMRadioFrequencyNumber::InputCapabilities
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TCoeInputCapabilities CFMRadioFrequencyNumber::InputCapabilities() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::InputCapabilities") ) );
	return TCoeInputCapabilities::EWesternNumericIntegerPositive;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::Text
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CFMRadioFrequencyNumber::Text() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::Text") ) );

	if (iFlags & EIsUninitialised)
		{
		return KNullDesC();
		}

	return *iText;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::SetValue
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyNumber::SetValue(TInt aValue, const CFont& aFont)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::SetValue") ) );
	iFlags &= ~EIsUninitialised;
	TPtr text = iText->Des();
	text.SetLength(0);
	TInt firstUnsetCharacter = text.Length();
	text.SetLength(firstUnsetCharacter + iMaxDigits);
	TInt lastUnsetCharacter = text.Length() - 1;

	if (aValue == 0)
		{
		text[lastUnsetCharacter--] = TText(iDigitType);
		}
	else
		{
		for (; aValue; aValue/=10)
			{	
			text[lastUnsetCharacter--] = (TText)(TText(iDigitType) + (aValue%10));
			}
		}
	
	if (lastUnsetCharacter >= firstUnsetCharacter)
		{
		if (iFlags & EFillWithLeadingZeros)
			{
			while (lastUnsetCharacter >= firstUnsetCharacter)
				{
				text[lastUnsetCharacter--] = TText(iDigitType);
				}
			}
		else
			{
			text.Delete(firstUnsetCharacter, (lastUnsetCharacter + 1) - firstUnsetCharacter);
			}
		}

	if (iFlags & EPreserveOldWidthBeforeEditing)
		{
		iMinimumWidthInPixels = aFont.TextWidthInPixels(Text());
		}
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::Value
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyNumber::Value() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::Value") ) );
    if (iText->Length() == 0)
        {
        CAknKeySoundSystem* soundPlayer = (static_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi()))->KeySounds();
		if (soundPlayer)
			{
			soundPlayer->PlaySound(EAvkonSIDWarningTone); 
			}
		CBaActiveScheduler::LeaveNoAlert();
        }

    return ValueFromText();
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::IsValid
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyNumber::IsValid() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::IsValid") ) );
	return (iText->Length() != 0);
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::IsEditable
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyNumber::IsEditable() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::IsEditable") ) );
	return ETrue;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::HighlightType
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEikMfneField::THighlightType CFMRadioFrequencyNumber::HighlightType() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::HighlightType") ) );
	return (iFlags & EIsBeingEditedWithCursor) ? ECursor : EInverseVideo;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::HandleKey
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyNumber::HandleKey(const CFont& /*aFont*/, const TKeyEvent& aKeyEvent, TBool /*aInterpretLeftAndRightAsEarEvents*/, TBool& aDataAltered, TInt& aHighlightIncrement)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::HandleKey") ) );
	TChar ch = aKeyEvent.iCode;
	TPtr text = iText->Des();
	TInt textLength = text.Length();
	
	switch (ch)
		{
		case EKeyLeftArrow:
		case EKeyRightArrow:
			{
			HandleLeftOrRightArrow(ch, aDataAltered, aHighlightIncrement);	
			break;
			}
		case EKeyBackspace:
			{
			if (textLength)
				{
				iFlags |= EIsBeingEditedWithCursor;
				text.SetLength(textLength-1);
				aDataAltered = ETrue;
				iFlags &= ~EIsUninitialised;
				}
			break;
			}
		default:
			{
			// Number key:
			if (ch.GetNumericValue() != KErrNotFound)
				{
				if (~iFlags & EIsBeingEditedWithCursor)
					{
					// If only one number field, do not change to cursor mode.
					if (iMaxDigits > 1)
						{
						iFlags |=  EIsBeingEditedWithCursor;
						}
					text.SetLength(0);
					}

				TBuf<1> normCh;
				normCh.Append(ch);
				NumberConversion::ConvertDigits(normCh, iDigitType);
				text.Append(normCh[0]);
				aDataAltered = ETrue;
				iFlags &= ~EIsUninitialised;
				if (iText->Length() >= iMaxDigits)
					{
					aHighlightIncrement = 1;
					}
				}
			break;
			}
		}
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::HandleDeHighlight
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyNumber::HandleDeHighlight(const CFont& aFont, CEikonEnv& aEikonEnv, TBool& aDataAltered, TBool& aError)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::HandleDeHighlight") ) );
	iFlags &= ~EIsBeingEditedWithCursor;

	if (iText->Length() == 0)
		{
		SetValue(iMaximumValue, aFont);
		aDataAltered = ETrue;
		aError = ETrue;
		aEikonEnv.InfoMsg(R_EIK_TBUF_NO_NUMBER_ENTERED);
		return;
		}

	TInt value = ValueFromText();
	TBuf<128> oldText = *iText;
	TBool unInit = iFlags & EIsUninitialised;
	SetValue(value, aFont);

	if (oldText != *iText)
		{
		aDataAltered = ETrue;
		}
	else
		{
		if (unInit)
			{
			iFlags |= EIsUninitialised;
			}
		else
			{
			iFlags &= ~EIsUninitialised;
			}
		}
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::ValueFromText
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyNumber::ValueFromText() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::ValueFromText") ) );
	if (!iText->Length())
		{
		return iMinimumValue;
		}

	TInt i = 0;
	TInt valueFromText = 0;
	TInt textLength = iText->Length();

	for (; i<textLength; ++i)
		{
		TText digit = (*iText)[i];
		valueFromText = (valueFromText * 10) + (TInt)(digit - TText(iDigitType));
		}

	return valueFromText;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::SetDigitType
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyNumber::SetDigitType(TDigitType aDigitType, const CFont& aFont)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::SetDigitType") ) );
	TInt value = Value();
	iDigitType = aDigitType;
	SetValue(value, aFont);
	}

// ---------------------------------------------------------
// CFMRadioFrequencyNumber::DigitType
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TDigitType CFMRadioFrequencyNumber::DigitType() const
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyNumber::DigitType") ) );
	return iDigitType;
	}

//  End of File  
