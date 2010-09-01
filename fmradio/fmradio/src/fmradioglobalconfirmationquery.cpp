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
* Description:  Source file of CFMRadioGlobalConfirmationQuery
*
*/


#include <AknGlobalConfirmationQuery.h>

#include "fmradiodefines.h"
#include "fmradioapp.h"
#include "fmradioglobalconfirmationquery.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::CFMRadioGlobalConfirmationQuery
// ---------------------------------------------------------------------------
//
CFMRadioGlobalConfirmationQuery::CFMRadioGlobalConfirmationQuery(MFMRadioGlobalConfirmationQueryObserver* aObserver)
    : CActive( CActive::EPriorityStandard ), iObserver(aObserver)
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioGlobalConfirmationQuery::ConstructL()
    {
    iAknGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::NewL
// ---------------------------------------------------------------------------
//
CFMRadioGlobalConfirmationQuery* CFMRadioGlobalConfirmationQuery::NewL(MFMRadioGlobalConfirmationQueryObserver* aObserver)
    {
    CFMRadioGlobalConfirmationQuery* self = CFMRadioGlobalConfirmationQuery::NewLC(aObserver);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::NewLC
// ---------------------------------------------------------------------------
//
CFMRadioGlobalConfirmationQuery* CFMRadioGlobalConfirmationQuery::NewLC(MFMRadioGlobalConfirmationQueryObserver* aObserver)
    {
    CFMRadioGlobalConfirmationQuery* self = NULL;
    self = new( ELeave ) CFMRadioGlobalConfirmationQuery(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::~CFMRadioGlobalConfirmationQuery
// ---------------------------------------------------------------------------
//
CFMRadioGlobalConfirmationQuery::~CFMRadioGlobalConfirmationQuery()
    {
    Cancel();
    delete iAknGlobalConfirmationQuery;
    delete iText;
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::ShowQueryL
// ---------------------------------------------------------------------------
//
void CFMRadioGlobalConfirmationQuery::ShowQueryL(const TDesC& aText, TInt aSoftkeys, TInt aAnimation/*, TInt aDialogId*/)
    {
    Cancel();
    delete iText;
    iText = NULL;
    iText = aText.AllocL();
    iAknGlobalConfirmationQuery->ShowConfirmationQueryL( iStatus, *iText, aSoftkeys, aAnimation );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::DoCancel
// ---------------------------------------------------------------------------
//
void CFMRadioGlobalConfirmationQuery::DoCancel()
    {
    if ( iAknGlobalConfirmationQuery )
        {
        iAknGlobalConfirmationQuery->CancelConfirmationQuery();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::RunL
// ---------------------------------------------------------------------------
//
void CFMRadioGlobalConfirmationQuery::RunL()
    {
    if ( iObserver )
        {
        iObserver->GlobalConfirmationQueryDismissedL(iStatus.Int());
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioGlobalConfirmationQuery::RunError
// ---------------------------------------------------------------------------
//
TInt CFMRadioGlobalConfirmationQuery::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }
