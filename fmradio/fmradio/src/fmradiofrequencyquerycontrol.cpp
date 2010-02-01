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
* Description: Manual tuning dialog for FMRadio
*
*/


// INCLUDE FILES
#include	<aknappui.h>
#include	<AknsFrameBackgroundControlContext.h>
#include    <aknbutton.h>
#include 	<AknLayoutDef.h>
#include 	<AknLayout2ScalableDef.h>
#include 	<aknlayoutscalable_avkon.cdl.h>
#include 	<avkon.mbg>
#include 	<avkon.rsg>
#include	<barsread.h>
#include	<eiklabel.h>
#include	<skinlayout.cdl.h>
#include	<AknsDrawUtils.h>
#include	<StringLoader.h>
#include 	<aknconsts.h>
#include	<fmradio.rsg>

#include    "fmradiofrequencyquerycontrol.h"
#include	"fmradiofrequencyeditor.h"
#include	"fmradio.hrh"
#include 	"debug.h"

// CONSTANTS

const TInt KFMRadioButtonLongPressDelay = 450; //microseconds after long press event is reported
const TInt KFMRadioButtonRepeatDelay = 500;  //microseconds after repeat event is reported
const TInt KFMRadioButtonRepeatInterval = 200; //microseconds after repeat event is repeated
const TInt KFMRadioButtonRepeatDelayMicroseconds(KFMRadioButtonRepeatDelay*1000);
const TInt KFMRadioButtonRepeatIntervalMicrosecondsAccelerated(KFMRadioButtonRepeatInterval*500);
const TInt KFMRadioButtonRepeatAccelerationDelay(2000000); //Two seconds until accelerate long press repeat

const TInt  KEditorFrameCustomLRMargins = 50;
const TInt  KEditorFrameCustomAdditionHeight = 30;

const TInt KFMRadioEditorCustomInnerFrameSize = 15;
const TInt KFMRadioEditorCustomButtonMarginToLeftFrame = 1;
const TInt KFMRadioEditorCustomButtonSize = 16;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CFMRadioFrequencyQueryControl::CFMRadioFrequencyQueryControl() : CAknQueryControl()
    {
    }
    
// Destructor
CFMRadioFrequencyQueryControl::~CFMRadioFrequencyQueryControl()
    {
    if (iFreqEditor)
        AknsUtils::DeregisterControlPosition(iFreqEditor);
    delete iFreqEditor;
	delete iMyPrompt;
	delete iMyEditorIndicator;
	delete iEditorContext;
	
	if ( iLongPressAccelerationTimer )
	    {
    	iLongPressAccelerationTimer->Cancel();
    	delete iLongPressAccelerationTimer;
	    }
	if ( iLongPressTimer) 
	    {
	    iLongPressTimer->Cancel();
	    delete iLongPressTimer;
	    }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::StaticCreateCustomControl
// ---------------------------------------------------------
//
SEikControlInfo CFMRadioFrequencyQueryControl::StaticCreateCustomControl(TInt aIdentifier)
{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyQueryControl::StaticCreateCustomControl -- ENTER") ) );
	SEikControlInfo customInfo;
	Mem::FillZ(&customInfo, sizeof(SEikControlInfo)); // zero all variables

	if (aIdentifier == EFMRadioCtFrequencyQuery)
	{
		customInfo.iControl = new CFMRadioFrequencyQueryControl;
	}

	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyQueryControl::StaticCreateCustomControl -- EXIT") ) );
	return customInfo;
}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::ConstructQueryL
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::ConstructQueryL(TResourceReader& aRes)
	{
	
	iFreqEditor = new (ELeave) CFMRadioFrequencyEditor;
    iFreqEditor->SetContainerWindowL(*this);
    iFreqEditor->ConstructFromResourceL(aRes);
    iFreqEditor->SetObserver(this);
    iFreqEditor->SetSkinTextColorL( EAknsCIQsnTextColorsCG27 );//query text input field

	iMyPrompt = new (ELeave) CEikLabel;
	iMyPrompt->SetContainerWindowL( *this );
	HBufC* promptTxt = StringLoader::LoadLC( R_QTN_FMRADIO_MANUAL_TUNE_FREQ );
	iMyPrompt->SetTextL( *promptTxt );
	CleanupStack::PopAndDestroy( promptTxt );

	iMyEditorIndicator = CFMRadioManualTuningEditorIndicator::NewL( this );
	
	// Construct editor context
    iEditorContext = CAknsFrameBackgroundControlContext::NewL(
        KAknsIIDQsnFrInput, TRect(0,0,1,1), TRect(0,0,1,1), EFalse );
    
    if ( AknLayoutUtils::PenEnabled() )
        {
        // Buttons with correct icons
          iIncreaseValueButton = 
                 CAknButton::NewL(KAvkonBitmapFile,
                 EMbmAvkonQgn_indi_button_increase,
                 EMbmAvkonQgn_indi_button_increase_mask,
                 -1,
                 -1,
                 EMbmAvkonQgn_indi_button_increase_pressed,
                 EMbmAvkonQgn_indi_button_increase_pressed_mask,
                 -1,
                 -1,
                 KNullDesC,
                 KNullDesC,
                 KAknButtonNoFrame /*| KAknButtonKeyRepeat*/ | KAknButtonReportOnLongPress,
                 0,
                 KAknsIIDQgnIndiButtonIncrease,
                 KAknsIIDNone,
                 KAknsIIDQgnIndiButtonIncreasePressed,
                 KAknsIIDNone );         
        iIncreaseValueButton->SetContainerWindowL(*this);
        iIncreaseValueButton->SetParent(this);
        iIncreaseValueButton->SetObserver(this);
        iIncreaseValueButton->MakeVisible(ETrue);
        iIncreaseValueButton->SetFocusing(EFalse);
       	iIncreaseValueButton->SetKeyRepeatInterval( KFMRadioButtonRepeatDelay, KFMRadioButtonRepeatInterval );
	    iIncreaseValueButton->SetLongPressInterval( KFMRadioButtonLongPressDelay );        

        iDecreaseValueButton = 
                 CAknButton::NewL(KAvkonBitmapFile,
                 EMbmAvkonQgn_indi_button_decrease,
                 EMbmAvkonQgn_indi_button_decrease_mask,
                 -1,
                 -1,
                 EMbmAvkonQgn_indi_button_decrease_pressed,
                 EMbmAvkonQgn_indi_button_decrease_pressed_mask,
                 -1,
                 -1,
                 KNullDesC,
                 KNullDesC,
                 KAknButtonNoFrame /*| KAknButtonKeyRepeat*/ | KAknButtonReportOnLongPress,
                 0,
                 KAknsIIDQgnIndiButtonDecrease,
                 KAknsIIDNone,
                 KAknsIIDQgnIndiButtonDecreasePressed,
                 KAknsIIDNone );
        iDecreaseValueButton->SetContainerWindowL(*this);
        iDecreaseValueButton->SetParent(this);
        iDecreaseValueButton->SetObserver(this);
        iDecreaseValueButton->MakeVisible(ETrue);
        iDecreaseValueButton->SetFocusing(EFalse);
		iDecreaseValueButton->SetKeyRepeatInterval( KFMRadioButtonRepeatDelay, KFMRadioButtonRepeatInterval );
	    iDecreaseValueButton->SetLongPressInterval( KFMRadioButtonLongPressDelay );
        }   
  
    iLongPressAccelerationTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iLongPressTimer = CPeriodic::NewL( CActive::EPriorityStandard );      
  	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CFMRadioFrequencyQueryControl::ComponentControl(TInt aIndex) const
	{
    /*lint -save -e1763*/
    CCoeControl* ccontrol = NULL;
    
	switch (aIndex)
		{
		case 0:
			{
			ccontrol = iMyPrompt;
			break;
			}
		case 1:
			{
			ccontrol = iMyEditorIndicator;
			break;
			}
		case 2:
			{
			ccontrol = iFreqEditor;
			break;
			}
		case 3:
			{
			if ( AknLayoutUtils::PenEnabled() )
				{
		    	ccontrol = iIncreaseValueButton;
		    	}
		   	break;
		   	}
		    
		case 4:
			{
			if ( AknLayoutUtils::PenEnabled() )
				{
	    		ccontrol = iDecreaseValueButton;
	    		}
	    	break;
	    	}
	    
		default:
			{
			ccontrol = NULL;
			break;
			}
		}
	return ccontrol;
    /*lint -restore*/
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::CountComponentControls
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyQueryControl::CountComponentControls() const
	{
	if ( AknLayoutUtils::PenEnabled() )
		{
		return 5;
		}
	else
		{
		return 3;
		}
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::FocusChanged
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::FocusChanged(TDrawNow aDrawNow)
	{
    if (iFreqEditor)
        {
        iFreqEditor->SetFocus(IsFocused(), aDrawNow);
        SizeChanged();
        }
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::PrepareForFocusLossL
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::PrepareForFocusLossL()
	{
    if (iFreqEditor)
        {
        iFreqEditor->PrepareForFocusLossL(); 
        }
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::SetAcceleratedLongPress
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::SetAcceleratedLongPress(TBool aAccelerated)
    {
    iLongPressAccelerationTimer->Cancel();
    iLongPressTimer->Cancel();
    
    if( aAccelerated)
        {
        //add more speed
        iLongPressTimer->Start(KFMRadioButtonRepeatIntervalMicrosecondsAccelerated,
                            KFMRadioButtonRepeatIntervalMicrosecondsAccelerated,
                            TCallBack(LongPressTimerCallBack, this));
        }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LongPressAccelerateCallBack
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyQueryControl::LongPressAccelerateCallBack( TAny* aAny )
    {
    ((CFMRadioFrequencyQueryControl*)(aAny))->SetAcceleratedLongPress(ETrue);
    return KErrNone;
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LongPressTimerCallBack
// ---------------------------------------------------------
//
TInt CFMRadioFrequencyQueryControl::LongPressTimerCallBack( TAny* aAny )
    {
    ((CFMRadioFrequencyQueryControl*)(aAny))->DoIncrementOrDecrement();
    return KErrNone;
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::DoIncrementOrDecrement
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::DoIncrementOrDecrement()
    {
    if(iChangeDirectionIncrease)
        {
        iFreqEditor->IncrementCurrentField();
        }
    else
        {
        iFreqEditor->DecrementCurrentField();
        }
    
    iKeyPressReported = ETrue;
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::StartKeypress
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::StartKeypress()
    {
    iKeyPressReported = EFalse;
    iLongPressAccelerationTimer->Cancel();
    iLongPressAccelerationTimer->Start(KFMRadioButtonRepeatAccelerationDelay, 0,
                                        TCallBack(LongPressAccelerateCallBack, this));
    iLongPressTimer->Start(KFMRadioButtonRepeatDelayMicroseconds,
                        KFMRadioButtonRepeatDelayMicroseconds,
                        TCallBack(LongPressTimerCallBack, this));
    iFreqEditor->SetEditorReportState( EFalse );
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CFMRadioFrequencyQueryControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType == EEventKeyDown &&
            (aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iScanCode == EStdKeyDownArrow) )
        {
        iChangeDirectionIncrease = (aKeyEvent.iScanCode == EStdKeyUpArrow);
        StartKeypress();
        
        return EKeyWasConsumed;
        }
    else if ( aType == EEventKeyUp &&
            (aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iScanCode == EStdKeyDownArrow) )
        {
        if(!iKeyPressReported)
            {
            //was not a long press
            DoIncrementOrDecrement();
            }
        
        SetAcceleratedLongPress(EFalse);
        iFreqEditor->SetEditorReportState( ETrue );
        iFreqEditor->DrawAndReportL( EFalse );
        
        return EKeyWasConsumed;
        }
    
    //eat all up and down events, this is for EEventKey
    if(aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iScanCode == EStdKeyDownArrow)
        {
        return EKeyWasConsumed;
        }
    
    return iFreqEditor->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::HandleControlEventL
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
	{
	if ( aEventType == CAknButton::ELongPressEndedEvent  )
		{
		SetAcceleratedLongPress(EFalse);
		iFreqEditor->SetEditorReportState( ETrue );
		iFreqEditor->DrawAndReportL( EFalse );
		}
	
	if ( (aControl == iIncreaseValueButton || aControl == iDecreaseValueButton )
	    && aEventType == CAknButton::ELongPressEvent )
        {
        iChangeDirectionIncrease = (aControl == iIncreaseValueButton);
        StartKeypress();
        return;
        }
    
	//single presses
	if ( aControl == iIncreaseValueButton && aEventType == EEventStateChanged )
        {
        iFreqEditor->IncrementCurrentField();
        return;
        }

	else if ( aControl == iDecreaseValueButton && aEventType == EEventStateChanged )
        {
        iFreqEditor->DecrementCurrentField();
        return;
        }
	
	//finally tune to the frequency
    if (iQueryControlObserver && aEventType == EEventStateChanged)
        {                
        if ( iFreqEditor->IsValidToReport() )
            {
			MAknQueryControlObserver::TQueryValidationStatus validity = MAknQueryControlObserver::EEditorValueValid;
			if ( !iFreqEditor->IsValid() )
				{
				validity = MAknQueryControlObserver::EEditorValueNotParsed; 
				}
            iQueryControlObserver->HandleQueryEditorStateEventL( this, MAknQueryControlObserver::EQueryControlEditorStateChanging, validity );
            }
		}	            
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::SetFrequency
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::SetFrequency(const TUint32 aFreq)
	{
	if (iFreqEditor)
		{
		iFreqEditor->SetFrequency(aFreq);
		}
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::Frequency
// ---------------------------------------------------------
//		
TUint32 CFMRadioFrequencyQueryControl::Frequency() const
	{
    if (iFreqEditor)
        {
        return iFreqEditor->Frequency();
        }
    return 0; 
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::ControlByLayoutOrNull
// ---------------------------------------------------------
//
CCoeControl* CFMRadioFrequencyQueryControl::ControlByLayoutOrNull(TInt /*aLayout*/)
	{
    return iFreqEditor;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::EditorContentIsValidL
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyQueryControl::EditorContentIsValidL() const
	{
	TInt ret = EFalse;
    if (iFreqEditor) 
        {
        CEikMfne* editor = iFreqEditor;
        ret = editor->Field(editor->CurrentField())->IsValid();
        }
    return ret;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::SetAndUseFlags
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::SetAndUseFlags(TBitFlags16 aFlags)
	{
	iFlags = aFlags;
	iMyEditorIndicator->SetFlags(aFlags);
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutEditor
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::LayoutEditor(const TLayoutMethod& aLayoutM)
	{
	TIndex LAFIndex(NbrOfPromptLines());
    iHasEditor = ETrue;
    LayoutEditorFrame(aLayoutM);
    LayoutEditorIndicator(aLayoutM);	

    CEikMfne* edwin = NULL;
    
    if ( iFreqEditor )
        {
        edwin = iFreqEditor;
        }        
 
    if ( edwin )
        {
        TInt variety( LAFIndex.PromptLine() );
        if ( AknLayoutUtils::PenEnabled() )
           {
           variety = NbrOfPromptLines() != 0 ? ( 6 - NbrOfPromptLines() ) : 5;
            }
        TAknWindowLineLayout lineLayout( AknLayoutScalable_Avkon::query_popup_pane( variety ) );
        TAknTextComponentLayout textLayout( AknLayoutScalable_Avkon::query_popup_pane_t1()  );        
        if ( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
        	{
        	lineLayout.it = iDialogSize.iHeight / 2 - lineLayout.iH / 2; 
        	}
      
        AknLayoutUtils::LayoutMfne( edwin, LayoutRect(), 
               TAknWindowComponentLayout::ComposeText( 
               		lineLayout,
                		textLayout ) );

       
        edwin->SetBorder( TGulBorder::ENone );
        if ( IsFocused() && !edwin->IsFocused() )
            {
            edwin->SetFocus( ETrue );
            }
        edwin->SetUpAndDownKeysConsumed( ETrue );
        }      
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutPrompt
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::LayoutPrompt(const TLayoutMethod& /*aLayoutM*/)
    {
	if (iMyPrompt)
		{
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    	TRgb color;
    	TInt error = AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG19 );
    	
		AknLayoutUtils::LayoutLabel(iMyPrompt, LayoutRect(), AKN_LAYOUT_TEXT_Code_query_pop_up_window_texts_Line_1(0) );
       
        if (!error)
        	{
            TRAP_IGNORE(AknLayoutUtils::OverrideControlColorL( *iMyPrompt, EColorLabelText, color ) )
        	}
		}

    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutImageOrAnim
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::LayoutImageOrAnim(const TLayoutMethod& /*aLayoutM*/)
    {
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutEditorFrame
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::LayoutEditorFrame(const TLayoutMethod& /*aLayoutM*/)
    {
    TIndex LAFIndex(NbrOfPromptLines());
    TRect rect = LayoutRect();
     
	TInt variety( LAFIndex.PromptLine() );
    TAknWindowLineLayout lineLayout(AKN_LAYOUT_WINDOW_Code_query_pop_up_window_elements_Line_1( variety ));
	lineLayout.il += KEditorFrameCustomLRMargins;
    lineLayout.ir += KEditorFrameCustomLRMargins;
    lineLayout.it -= KEditorFrameCustomAdditionHeight/2; 
    lineLayout.ib -= KEditorFrameCustomAdditionHeight/2;
    lineLayout.iH += KEditorFrameCustomAdditionHeight;
	if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
		{
    	lineLayout.it = iDialogSize.iHeight/ 2 - lineLayout.iH / 2;
    	}         
	iEditorVerShadow.LayoutRect( rect, lineLayout );

	lineLayout = AKN_LAYOUT_WINDOW_Code_query_pop_up_window_elements_Line_2( variety );
	lineLayout.il += KEditorFrameCustomLRMargins;
    lineLayout.ir += KEditorFrameCustomLRMargins;
    lineLayout.it -= KEditorFrameCustomAdditionHeight/2; 
    lineLayout.ib -= KEditorFrameCustomAdditionHeight/2;
    lineLayout.iH += KEditorFrameCustomAdditionHeight;
	if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
		{
    	lineLayout.it = iDialogSize.iHeight/ 2 - lineLayout.iH / 2; 
    	}     
	iEditorHorShadow.LayoutRect( rect, lineLayout );
    
	// variety needs to be adjusted for touch layouts.
    if ( AknLayoutUtils::PenEnabled() )
        {
        variety = NbrOfPromptLines() != 0 ? ( 6 - NbrOfPromptLines() ) : 5;    
        }
	lineLayout = AknLayoutScalable_Avkon::query_popup_pane( variety );
	lineLayout.il += KEditorFrameCustomLRMargins;
    lineLayout.ir += KEditorFrameCustomLRMargins;
    lineLayout.it -= KEditorFrameCustomAdditionHeight/2; 
    lineLayout.ib -= KEditorFrameCustomAdditionHeight/2;
    lineLayout.iH += KEditorFrameCustomAdditionHeight;
	if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
		{
    	lineLayout.it = iDialogSize.iHeight/ 2 - lineLayout.iH / 2;  
    	}     
    iEditorFrame.LayoutRect( rect, lineLayout );
    iFreqEditor->SetEditorFrameRect( iEditorFrame.Rect() ); 
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutEditorIndicator
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::LayoutEditorIndicator(const TLayoutMethod& /*aLayoutM*/)
    { 
    TIndex LAFIndex(NbrOfPromptLines());

    if (!iMyEditorIndicator)
        return;

    TInt variety( LAFIndex.PromptLine() );
    if ( AknLayoutUtils::PenEnabled() )
        {
        // adjust layout variety for touch
        variety = 5 - variety;
        }
    TAknWindowLineLayout lineLayout(AknLayoutScalable_Avkon::indicator_popup_pane( variety ));
    lineLayout.ir += KEditorFrameCustomLRMargins;
    lineLayout.it -= KEditorFrameCustomAdditionHeight/2;
    if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
    	{
    	lineLayout.it = iDialogSize.iHeight / 2 - lineLayout.iH - iEditorFrame.Rect().Height()/2; 
    	}     
    AknLayoutUtils::LayoutControl(iMyEditorIndicator, LayoutRect(), lineLayout );
   }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::LayoutRect
// ---------------------------------------------------------
//
TRect CFMRadioFrequencyQueryControl::LayoutRect() 
    {  
	TPoint topLeft = Position();
    TRect parent( TPoint(0,0), iDialogSize );
	TAknLayoutRect layout;

	layout.LayoutRect(parent, AKN_LAYOUT_WINDOW_Note_pop_up_window_graphics_Line_5(parent));

	TRect rect(layout.Rect());
	topLeft.iX -= rect.iTl.iX;
    topLeft.iY -= rect.iTl.iY;

    TPoint bottomRight (topLeft);
    TSize  size (iDialogSize);
    bottomRight.iY += size.iHeight; 
    bottomRight.iX += size.iWidth;
    return TRect(topLeft,bottomRight);		
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::SetLineWidthsL
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::SetLineWidthsL() 
    {    
    if (!iLineWidths)
        return;
    
    iLineWidths->Reset();  
    TAknWindowLineLayout lineLayout(AknLayoutScalable_Avkon::popup_query_code_window(0));
    if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
    lineLayout.it += KFMRadioFrequencyQueryDialogCustomAdditionHeight / 2; 
    
    TAknLayoutRect parentLayoutRect;
    parentLayoutRect.LayoutRect(iAvkonAppUi->ClientRect(), lineLayout);
     
    TAknLayoutText textRect;
    for (TInt i = 0; i < 3; i++)
        {  
        TAknTextLineLayout textLineLayout = AKN_LAYOUT_TEXT_Code_query_pop_up_window_texts_Line_1(i);
        if ( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
        	{
          	textLineLayout.iB -= KFMRadioFrequencyQueryDialogCustomAdditionHeight / 2; 
        	}
        textRect.LayoutText(parentLayoutRect.Rect(), textLineLayout);
        iLineWidths->AppendL(textRect.TextRect().Width());
        }   
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::Draw
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc=SystemGc(); 
    TRect rect(Rect());
    
    TRect dialogRect(TPoint(0,0), iDialogSize );
    rect.iBr.iY += dialogRect.iBr.iY; 

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TRgb color;
    TInt error = AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG19 );
    if ( !error )
        {        
        TRAP_IGNORE(AknLayoutUtils::OverrideControlColorL( *(CAknQueryControl*)this, EColorLabelText, color ) );
        }

    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, rect ); 
    DrawFrequencyEditorFrame(gc,rect);     
    }

/**
 * Draw frequency editor frame and shadow 
 */
void CFMRadioFrequencyQueryControl::DrawFrequencyEditorFrame(CWindowGc& aGc,TRect& /*aRect*/) const
    {
    if (iHasEditor)
        {
        TBool skinnedDraw( EFalse );     
        skinnedDraw = AknsDrawUtils::Background( 
                AknsUtils::SkinInstance(), 
                iEditorContext, 
                aGc, 
                iEditorFrame.Rect() );  
        
        if( !skinnedDraw )
            {
            iEditorFrame.DrawOutLineRect(aGc);
            iEditorVerShadow.DrawRect(aGc); 
            iEditorHorShadow.DrawRect(aGc);
            }
        }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::SizeChanged.
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::SizeChanged()
    {  
	if (iMyPrompt)
		{
		iMyPrompt->SetRect(LayoutRect());
		LayoutPrompt(ETimeQueryLayoutM);
		}
    if (iFreqEditor)
		{
		LayoutEditor(ETimeQueryLayoutM);
		}
			
    if( iHasEditor && iEditorContext )
        {
        // Layout editor context
        TAknLayoutRect topLeft;
        TAknLayoutRect bottomRight;
        
        TAknWindowLineLayout lineLayout(SkinLayout::Input_field_skin_placing__general__Line_2());
        topLeft.LayoutRect(iEditorFrame.Rect(), lineLayout );                    
        bottomRight.LayoutRect(TRect(iEditorFrame.Rect().iBr, iEditorFrame.Rect().iBr), SkinLayout::Input_field_skin_placing__general__Line_5());
		
        TRect outerRect = TRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
        TRect innerRect = TRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);
        
        outerRect.iTl.iY -= KFMRadioEditorCustomInnerFrameSize;
        outerRect.iBr.iY += KFMRadioEditorCustomInnerFrameSize;				
		
		innerRect.iTl.iY -= KFMRadioEditorCustomInnerFrameSize;
        innerRect.iBr.iY += KFMRadioEditorCustomInnerFrameSize;	
				
        iEditorContext->SetFrameRects( outerRect, innerRect );
        // Chain with the background (since the frame doesn't occupy the entire
        // layout and it may even be transparent)
        iEditorContext->SetParentContext( AknsDrawUtils::ControlContextOfParent( this ) );
        
        if ( AknLayoutUtils::PenEnabled() )
            {
            if ( !iIncreaseValueButton )
                {
                TRAPD( err, iIncreaseValueButton = 
                         CAknButton::NewL(KAvkonBitmapFile,
                         EMbmAvkonQgn_indi_button_increase,
                         EMbmAvkonQgn_indi_button_increase_mask,
                         -1,
                         -1,
                         EMbmAvkonQgn_indi_button_increase_pressed,
                         EMbmAvkonQgn_indi_button_increase_pressed_mask,
                         -1,
                         -1,
                         KNullDesC,
                         KNullDesC,
                         KAknButtonNoFrame | KAknButtonKeyRepeat | KAknButtonReportOnLongPress,
                         0,
                         KAknsIIDQgnIndiButtonIncrease,
                         KAknsIIDNone,
                         KAknsIIDQgnIndiButtonIncreasePressed,
                         KAknsIIDNone );
						iIncreaseValueButton->SetContainerWindowL(*this);
                );
                
                if ( err == KErrNone )
                	{
	                iIncreaseValueButton->SetParent(this);
	                iIncreaseValueButton->SetObserver(this);
	                iIncreaseValueButton->MakeVisible(ETrue);
	                iIncreaseValueButton->SetFocusing(EFalse);
					iIncreaseValueButton->SetKeyRepeatInterval( KFMRadioButtonRepeatDelay, KFMRadioButtonRepeatInterval );
		    		iIncreaseValueButton->SetLongPressInterval( KFMRadioButtonLongPressDelay );
		    		}
                }
                
            if ( !iDecreaseValueButton )
                {
                TRAPD( err, iDecreaseValueButton = CAknButton::NewL(KAvkonBitmapFile,
											                         EMbmAvkonQgn_indi_button_decrease,
											                         EMbmAvkonQgn_indi_button_decrease_mask,
											                         -1,
											                         -1,
											                         EMbmAvkonQgn_indi_button_decrease_pressed,
											                         EMbmAvkonQgn_indi_button_decrease_pressed_mask,
											                         -1,
											                         -1,
											                         KNullDesC,
											                         KNullDesC,
											                         KAknButtonNoFrame | KAknButtonKeyRepeat | KAknButtonReportOnLongPress,
											                         0,
											                         KAknsIIDQgnIndiButtonDecrease,
											                         KAknsIIDNone,
											                         KAknsIIDQgnIndiButtonDecreasePressed,
											                         KAknsIIDNone );
    	            		iDecreaseValueButton->SetContainerWindowL(*this); 
                );
                
                if ( err == KErrNone )
                	{
                iDecreaseValueButton->SetParent(this);
                iDecreaseValueButton->SetObserver(this);
                iDecreaseValueButton->MakeVisible(ETrue);
                iDecreaseValueButton->SetFocusing(EFalse);
				iDecreaseValueButton->SetKeyRepeatInterval( KFMRadioButtonRepeatDelay, KFMRadioButtonRepeatInterval );
	    		iDecreaseValueButton->SetLongPressInterval( KFMRadioButtonLongPressDelay );
		    		}
            	}
            // Position the buttons according to LAF
            
            TIndex LAFIndex(NbrOfPromptLines());
            TInt variety( 5 - LAFIndex.PQCWindow() );
            
            AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
            TInt offset = 0;
            if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight)
                {
                offset = 3;
                }
            else if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
                {
                offset = 6;
                }
            variety += offset;    

            TAknWindowComponentLayout btnSpaceLayout;            
            
            btnSpaceLayout = AknLayoutScalable_Avkon::button_value_adjust_pane( variety );
              
            if ( iIncreaseValueButton )
                {
    			TAknWindowLineLayout buttonIncr = TAknWindowComponentLayout::Compose(
				    btnSpaceLayout,
				    AknLayoutScalable_Avkon::button_value_adjust_pane_g1()).LayoutLine();

				TInt marginToNumberFrame = KEditorFrameCustomLRMargins - 
				KFMRadioEditorCustomButtonMarginToLeftFrame - KFMRadioEditorCustomButtonSize;
																    
    			buttonIncr.ir += marginToNumberFrame;
    			buttonIncr.iH += KFMRadioEditorCustomButtonSize;
    			buttonIncr.iW += KFMRadioEditorCustomButtonSize;
    			
    			if ( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
					{
    				buttonIncr.it = iDialogSize.iHeight / 2 - buttonIncr.iH;  
    				}
    				
                TAknLayoutRect increaseValueButtonRect;
                increaseValueButtonRect.LayoutRect( LayoutRect(), buttonIncr ); 
                iIncreaseValueButton->SetRect(increaseValueButtonRect.Rect());
                }
                
            if ( iDecreaseValueButton) 
                {
    			TAknWindowLineLayout buttonDecr = TAknWindowComponentLayout::Compose(
				    btnSpaceLayout,
				    AknLayoutScalable_Avkon::button_value_adjust_pane_g2()).LayoutLine();
				    
				TInt marginToNumberFrame = KEditorFrameCustomLRMargins - 
				KFMRadioEditorCustomButtonMarginToLeftFrame - KFMRadioEditorCustomButtonSize;
																    
    			buttonDecr.ir += marginToNumberFrame;
    			buttonDecr.iH += KFMRadioEditorCustomButtonSize;
    			buttonDecr.iW += KFMRadioEditorCustomButtonSize;
    			
    			if ( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
    				{
    				buttonDecr.it = iDialogSize.iHeight / 2;  
    				}
                TAknLayoutRect decreaseValueButtonRect;
                decreaseValueButtonRect.LayoutRect( LayoutRect(), buttonDecr ); 
                iDecreaseValueButton->SetRect(decreaseValueButtonRect.Rect());
                }
            }    
        }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::MopSupplyObject
// ---------------------------------------------------------
//
TTypeUid::Ptr CFMRadioFrequencyQueryControl::MopSupplyObject(TTypeUid aId)
    {    
    if( (aId.iUid == MAknsControlContext::ETypeId) && 
        iEditorContext && iHasEditor )
        {
        // Return specific context iff editor exists and the context
        // has been constructed.
        return MAknsControlContext::SupplyMopObject( 
            aId, iEditorContext );
        }

    if ( aId.iUid == CAknQueryControl::ETypeId )
    	{
    	return aId.MakePtr( this );
    	}
    
    return SupplyMopObject(aId, iMyEditorIndicator);
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::WindowLayout
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::WindowLayout( TAknWindowLineLayout& aLayout ) const
    {
    TIndex LAFIndex(NbrOfPromptLines());
  
	aLayout = AknLayoutScalable_Avkon::popup_query_code_window(LAFIndex.PQCWindow());
	if( KFMRadioFrequencyQueryDialogCustomAdditionHeight )
		{
		aLayout.iH += KFMRadioFrequencyQueryDialogCustomAdditionHeight; 
		}
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::CanLeaveEditorL
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyQueryControl::CanLeaveEditorL() 
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyQueryControl::CanLeaveEditorL") ) );
	
    if (iFreqEditor)
		{
		return iFreqEditor->CanLeaveEditorL();
		}
	return ETrue;
    }


/*********************************
 * CFMRadioManualTuningEditorIndicator
 *********************************/

CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator* CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::NewL(CCoeControl* aControl)
    {
    CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator* self = new(ELeave)CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator();
    CleanupStack::PushL(self);
    self->ConstructL(aControl);
    CleanupStack::Pop(self);
    return self;
    }

CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::~CFMRadioManualTuningEditorIndicator()
    {
    }

CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::CFMRadioManualTuningEditorIndicator() : CAknIndicatorContainer(CAknIndicatorContainer::EQueryEditorIndicators)
    {
    }

void CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::ConstructL(CCoeControl* aControl)
    {
    CreateWindowL(aControl);
    
	TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_AVKON_NAVI_PANE_EDITOR_INDICATORS);
    ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();  // resource reader

    SetExtent(TPoint(0,0), TSize(0,0));
    MakeVisible( EFalse );
    //ActivateL();
    }

void CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::SetState(TAknEditingState aState)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::SetState (%d)"), (int)aState ) );
	if (AknLayoutUtils::Variant() == EEuropeanVariant)
		{    
		SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorNumberCase), (aState == ENumeric) ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);    
		SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorArabicIndicNumberCase), (aState == EArabicIndicNumeric) ? EAknIndicatorStateOn : EAknIndicatorStateOff, EFalse);
		SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorQuery), (aState == EStateNone)  ? EAknIndicatorStateOff : EAknIndicatorStateOn, EFalse);
		}
	else
		{
        SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorNumberCase), (aState == ENumeric) ? EAknIndicatorStateOn : EAknIndicatorStateOff,  EFalse);    
		SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorQuery), (aState == EStateNone)  ? EAknIndicatorStateOff : EAknIndicatorStateOn, EFalse);
		}
	
	MakeVisible( EFalse );
	//DrawDeferred();
	}

CAknIndicatorContainer* CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::IndicatorContainer()
    {
    return this;
    }

void CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::SetFlags(TBitFlags16 aFlags)
    {
    iBitFlags = aFlags;
    }

void CFMRadioFrequencyQueryControl::CFMRadioManualTuningEditorIndicator::Reserved_1()
	{
	}

/*****************************************************************
 * CFMRadioFrequencyQueryControl::TIndex
 *
 * Manage indexes into LAF tables
 *
 * PN stands for "Popup Note"
 *
 * PQD stands for "Popup Query Data'
 *
 * PQC stands for "Popup Query Code'
 *
 * 'DPQ' stands for "Data Query Popup"
 ******************************************************************/

  
CFMRadioFrequencyQueryControl::TIndex::TIndex(TInt aNumberOfPromptLines)
    : iNumberOfPromptLines(aNumberOfPromptLines)
    {
    }
        
/**
 * Return index into LAF tables that depend directly on number of prompt lines,
 * index is equal to number of prompt lines minus one unless there are no prompt
 * lines in which case the index is zero
 */
TInt CFMRadioFrequencyQueryControl::TIndex::PromptLine() const
    {
    return iNumberOfPromptLines > 0 ? iNumberOfPromptLines - 1 : 0;
    }

/**
 * Return index into Main Pane PQDC Window, which
 * depends on total number of prompt lines minus 2, e.g.
 * index 0 for 2 prompt lines total.
 *
 */
TInt CFMRadioFrequencyQueryControl::TIndex::DQPWindowTextsLine2(TInt aLineNum) const
    {
    static const TInt  KDataQueryPopupWindowTextsLine2Index[3][5] = 
    {   {0,  1, 2, 3, 4},    //1 PROMPT LINE
        {5,  6, 7, 8, 9},    //2 PROMPT LINES
        {10, 10, 10, 10, 10}  }; //3 PROMPT LINES
    
    return KDataQueryPopupWindowTextsLine2Index[PromptLine()][aLineNum-1];
    }

TInt CFMRadioFrequencyQueryControl::TIndex::PNWindow() const
    {     
    static const TInt KPopupNoteWindowIndex[5] = { 0,0,0,1,2 };
    
    AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
    if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight)
        {
        // variety numbers for right CBA are 3,4 and 5 
        return (KPopupNoteWindowIndex[PromptLine()] + 3);  
        }
    else if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
        {
        /// variety numbers for left CBA are 6,7 and 8 
        return (KPopupNoteWindowIndex[PromptLine()] + 6);
        }
    else // bottom
        {
        return KPopupNoteWindowIndex[PromptLine()];
        }
    }

TInt CFMRadioFrequencyQueryControl::TIndex::PQDWindow(TInt aLineNum) const
    {
    AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
    
    if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight)
        {
        static const TInt KPopupQueryDataWindowIndex[7] = 
            {13, 12, 11, 10, 9, 8, 7}; // variety numbers for right CBA are 7-13 
        return KPopupQueryDataWindowIndex[PromptLine()+aLineNum-1];
        }
    else if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
        {
        static const TInt KPopupQueryDataWindowIndex[7] = 
            {20, 19, 18, 17, 16, 15, 14}; // variety numbers for left CBA are 14-20 
        return KPopupQueryDataWindowIndex[PromptLine()+aLineNum-1];
        }
    else // bottom
        {
        static const TInt KPopupQueryDataWindowIndex[7] = 
            {0, 1, 2, 3, 4, 5, 6}; // variety numbers for bottom CBA are 0-6
        return KPopupQueryDataWindowIndex[PromptLine()+aLineNum-1];
        }
    }
    
TInt CFMRadioFrequencyQueryControl::TIndex::PQCWindow() const
    {    
    AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
    
    if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight)
        {
        // variety numbers for right CBA are 3,4 and 5 
        return iNumberOfPromptLines > 0 ? (iNumberOfPromptLines - 1 + 3) : 3;
        }
    else if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
        {
        // variety numbers for left CBA are 6,7 and 8 
        return iNumberOfPromptLines > 0 ? (iNumberOfPromptLines - 1 + 6) : 6;
        }
    else // bottom
        {
        // variety numbers for bottom CBA are 0,1 and 2 
        return iNumberOfPromptLines > 0 ? (iNumberOfPromptLines - 1) : 0;
        }
    }    

// ---------------------------------------------------------
// CFMRadioFrequencyQueryControl::HandlePointerEventL
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    iFreqEditor->HandlePointerEventL( aPointerEvent );
    }

//  End of File  
