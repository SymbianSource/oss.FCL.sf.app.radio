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
#include	<aknborders.h>
#include	<eikcapc.h>

#include 	<aknlayoutscalable_avkon.cdl.h>

#include    "fmradiofrequencyquerydialog.h"
#include	"fmradiofrequencyquerycontrol.h"
#include	"fmradio.hrh"
#include 	"debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CFMRadioFrequencyQueryDialog::CFMRadioFrequencyQueryDialog(TUint32& aFreq, const TTone& aTone) : CAknQueryDialog(aTone), iFreq(aFreq)
    {
    }

// Two-phased constructor.
CFMRadioFrequencyQueryDialog* CFMRadioFrequencyQueryDialog::NewL(TUint32& aFreq, const TTone& aTone)
    {
    CFMRadioFrequencyQueryDialog* self = new (ELeave) CFMRadioFrequencyQueryDialog(aFreq, aTone);
	CleanupStack::PushL(self);
	CEikonEnv::Static()->AddLibraryL(CFMRadioFrequencyQueryControl::StaticCreateCustomControl);
	CleanupStack::Pop(self);
    return self;
    }

    
// Destructor
CFMRadioFrequencyQueryDialog::~CFMRadioFrequencyQueryDialog()
    {
	CEikonEnv::Static()->RemoveLibrary(CFMRadioFrequencyQueryControl::StaticCreateCustomControl);
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::QueryControl
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknQueryControl* CFMRadioFrequencyQueryDialog::QueryControl() const
    {
	CEikCaptionedControl* controlPtr = NULL;
    const TInt KMaxNumPages = GetNumberOfPages();
    for (TInt pageCount=0; pageCount<KMaxNumPages; pageCount++)
        {
        const TInt KMaxNumLinesOnThisPage= GetNumberOfLinesOnPage(pageCount);
        for (TInt lineCount=0; lineCount< KMaxNumLinesOnThisPage; lineCount++)
            {
            controlPtr = GetLineByLineAndPageIndex(lineCount, pageCount);
            if (controlPtr && controlPtr->iControlType == EFMRadioCtFrequencyQuery) 
                {
                return static_cast<CAknQueryControl*>(controlPtr->iControl);
                }
            }
        }
    return 0;
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::PreLayoutDynInitL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryDialog::PreLayoutDynInitL()
    {
   // SetBorder(AknBorderId::EAknBorderNotePopup);
    SetBorder( TGulBorder::ENone );
    SetEditableL(ETrue) ;

    CFMRadioFrequencyQueryControl* control = static_cast<CFMRadioFrequencyQueryControl*>(QueryControl());
    if (control)
        {
        control->SetQueryControlObserver(this);
        control->SetFrequency(iFreq);
        }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::HandleQueryEditorStateEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyQueryDialog::HandleQueryEditorStateEventL(CAknQueryControl* aQueryControl, TQueryControlEvent aEventType, TQueryValidationStatus aStatus)
	{
	FTRACE( FPrint( _L(" *** FM Radio -- CFMRadioFrequencyQueryDialog::HandleQueryEditorStateEventL") ) );
	CAknQueryDialog::HandleQueryEditorStateEventL(aQueryControl, aEventType, aStatus);
	CFMRadioFrequencyQueryControl* control = static_cast<CFMRadioFrequencyQueryControl*>(aQueryControl);
	if ((control) && (aStatus == MAknQueryControlObserver::EEditorValueValid) && (iFreq != control->Frequency()))
		{
		iFreq = control->Frequency();
		ReportEventL(EEventStateChanged);
		}
	return ETrue;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::Frequency
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUint32 CFMRadioFrequencyQueryDialog::Frequency() const
	{
	return iFreq;
	}

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::UpdateLeftSoftKeyL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioFrequencyQueryDialog::UpdateLeftSoftKeyL()
    {
    CFMRadioFrequencyQueryControl* control = static_cast<CFMRadioFrequencyQueryControl*>(QueryControl());
    if ( control )
        {
        TBool showCommand = control->EditorContentIsValidL();
        ButtonGroupContainer().MakeCommandVisibleByPosition( CEikButtonGroupContainer::ELeftSoftkeyPosition, showCommand );
        ButtonGroupContainer().MakeCommandVisibleByPosition( CEikButtonGroupContainer::EMiddleSoftkeyPosition, showCommand );
        ButtonGroupContainer().DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CFMRadioFrequencyQueryDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CFMRadioFrequencyQueryDialog::OkToExitL(TInt /*aButtonId*/)
	{
	CFMRadioFrequencyQueryControl* control = static_cast<CFMRadioFrequencyQueryControl*>(QueryControl());
	return control->CanLeaveEditorL();
	}

//  End of File  
