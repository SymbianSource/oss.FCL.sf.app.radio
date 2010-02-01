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
* Description: Gets country code and network info from mobile network
*
*/


// INCLUDES
#ifdef __WINS__
#include <centralrepository.h>
#else
#include <commsdattypesv1_1.h>
#include <etel.h>
#include <etelmm.h>
#include <commsdat_partner.h>
#endif

#include "fmradionetworkchangeobserver.h"
#include "fmradiomobilenetworkinfolistener.h"
#include "fmradioenginecrkeys.h"

using namespace CommsDat;


#ifdef __WINS__
// Default size for the cenrep query
const TInt KFMRadioDefaultNetworkInfoBufferSize = 0x80; 
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
#ifdef __WINS__
CFMRadioMobileNetworkInfoListener::CFMRadioMobileNetworkInfoListener(MFMRadioNetworkChangeObserver & aObserver) : 
    CActive(CActive::EPriorityStandard), iNetworkChangeObserver(aObserver)
#else
CFMRadioMobileNetworkInfoListener::CFMRadioMobileNetworkInfoListener(MFMRadioNetworkChangeObserver & aObserver) : 
    CActive(CActive::EPriorityStandard), iNetworkChangeObserver(aObserver), iNetworkInfo(), iNetworkInfoPckg(iNetworkInfo)
#endif
    {
    }

// EPOC default constructor can leave.
void CFMRadioMobileNetworkInfoListener::ConstructL()
    {

#ifdef __WINS__
    iSubscriberId = KNullDesC().AllocL();
    
    TRAPD( err, iRepository = CRepository::NewL( KCRUidFMRadioEngine ) )
    if ( err == KErrNotFound )
        {
        // Central Repository not found, so don't request a notification
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }
    else 
        {
        // Read initial values from Central Repository
        GetCenRepNetworkInfoL();
		SaveNetworkInfo();
        // Start listening
        CActiveScheduler::Add( this );
        User::LeaveIfError( iRepository->NotifyRequest( KFMRadioCREmulatorNetworkInfo, iStatus ) );
        SetActive();
        }
#else
	// Get default TSY from database.
		
	CMDBSession * db = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(db);
	
	CMDBRecordSet<CCDModemBearerRecord> * set = new (ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
	CleanupStack::PushL(set);
			
	set->LoadL(*db);
	
	TInt index = 0;	
	CCDModemBearerRecord * record = static_cast<CCDModemBearerRecord*>(set->iRecords[index++]);
	
	if(record->iTsyName.IsNull() && set->iRecords.Count() > index) // the first item is normally only a template
		{
		record = static_cast<CCDModemBearerRecord*>(set->iRecords[index++]);
		}

	TPtrC name(static_cast<const TDesC&>(record->iTsyName));
	TBuf<KMaxTextLength> tsyName;
	tsyName.Copy(name);
	
	CleanupStack::PopAndDestroy(set);
	CleanupStack::PopAndDestroy(db);	

	//  Open phone 
	User::LeaveIfError(iTelServer.Connect());

	// TSY module gets automatically unloaded when tel.Close() is called ( and tel is is CU-stack),
	// so loaded TSY is also leave-safe
	User::LeaveIfError(iTelServer.LoadPhoneModule(tsyName));
	
	// Get number of phones.
	TInt phones(0);
	User::LeaveIfError(iTelServer.EnumeratePhones(phones));

	// Get phone info of first legal phone.
	TInt legalPhoneIndex = KErrNotFound;
	RTelServer::TPhoneInfo phoneInfo;
	for (TInt i=0; i<phones && legalPhoneIndex == KErrNotFound; i++)
		{
		if (iTelServer.GetPhoneInfo(i, phoneInfo) == KErrNone)
			{
			if (phoneInfo.iNetworkType == RTelServer::ENetworkTypeMobileDigital)
				{
				legalPhoneIndex = i;
				}
			}
		}
	User::LeaveIfError(legalPhoneIndex);

	// Open legal phone.
	User::LeaveIfError(iPhone.Open(iTelServer, phoneInfo.iName));

	TUint32 networkCaps;
	User::LeaveIfError(iPhone.GetNetworkCaps(networkCaps));
	TUint32 identityCaps;
	User::LeaveIfError(iPhone.GetIdentityCaps(identityCaps));
	// Check if we are allowed to get network info.
	if (networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
		{
		// Gather initial information synchronically.
		iPhone.GetCurrentNetwork(iStatus, iNetworkInfoPckg, iLocation);
		User::WaitForRequest(iStatus);
		User::LeaveIfError(iStatus.Int());
		SaveNetworkInfo();
		// Start first notify request (if allowed).
		if (networkCaps & RMobilePhone::KCapsNotifyCurrentNetwork)
			{
			CActiveScheduler::Add(this);
			iPhone.NotifyCurrentNetworkChange(iStatus, iNetworkInfoPckg, iLocation);
			SetActive();
			}
		}
	else
		{
		// Leave if we are not allowed to get network info.
		// Other action could also be considered!!!!
		User::Leave(KErrNotSupported);
		}
	if(identityCaps & RMobilePhone::KCapsGetSubscriberId)
		{
		TRequestStatus stat;
		RMobilePhone::TMobilePhoneSubscriberId subId;
		iPhone.GetSubscriberId(stat, subId);
		User::WaitForRequest(stat);
		if(!stat.Int())
			{
			iSubscriberId = subId.AllocL();
			}
		else
			{
			iSubscriberId = KNullDesC().AllocL();
			}
		}
#endif	
  }


// Two-phased constructor.
CFMRadioMobileNetworkInfoListener* CFMRadioMobileNetworkInfoListener::NewL(MFMRadioNetworkChangeObserver & aObserver)
    {
    CFMRadioMobileNetworkInfoListener* self = new (ELeave) CFMRadioMobileNetworkInfoListener(aObserver);   
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// Destructor
CFMRadioMobileNetworkInfoListener::~CFMRadioMobileNetworkInfoListener()
	{
	Cancel();
#ifdef __WINS__
    delete iRepository;
    delete iEmulatorNetworkInfo;
#else
    iPhone.Close();
    iTelServer.Close();
#endif
    delete iSubscriberId;
	}


// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::RunL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioMobileNetworkInfoListener::RunL()
	{
#ifdef __WINS__
    GetCenRepNetworkInfoL();
    SaveNetworkInfo();

    User::LeaveIfError( iRepository->NotifyRequest( KFMRadioCREmulatorNetworkInfo, iStatus ) );
    SetActive();
#else
	TInt statusCode = iStatus.Int();
	if (statusCode == KErrNone)		// No other codes are returned (except error).
		{
		SaveNetworkInfo();
		}
	// Start new notify request.
	iPhone.NotifyCurrentNetworkChange(iStatus, iNetworkInfoPckg, iLocation);
	SetActive();
#endif
	}


// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::DoCancel
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioMobileNetworkInfoListener::DoCancel()
	{
#ifdef __WINS__
    iRepository->NotifyCancel( KFMRadioCREmulatorNetworkInfo );
#else
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
#endif
	}

// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::SaveNetworkInfo
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CFMRadioMobileNetworkInfoListener::SaveNetworkInfo()
	{
#ifdef __WINS__
    // iEmulatorNetworkInfo contains country code + network ID
    if ( iEmulatorNetworkInfo && iEmulatorNetworkInfo->Length() > KFMRadioCREmulatorCountryCodeLength )
        {
        iCountryCode.Copy( iEmulatorNetworkInfo->Left( KFMRadioCREmulatorCountryCodeLength ) );
        iNetworkId.Copy( iEmulatorNetworkInfo->Mid( KFMRadioCREmulatorCountryCodeLength, 
            Min( iEmulatorNetworkInfo->Length() - KFMRadioCREmulatorCountryCodeLength, 
                 iNetworkId.MaxLength() ) ) );
        }
    else
        {
        iCountryCode.Zero();
        iNetworkId.Zero();
        }
#else
	iNetworkInfo = iNetworkInfoPckg();
	iNetworkId.Copy(iNetworkInfo.iNetworkId);
	iCountryCode.Copy(iNetworkInfo.iCountryCode);
#endif

	// Create combine network ID and country code.
	TBuf<20> compNetworkId;
	compNetworkId.Append(iCountryCode);
	// have to add '0' digit to the string to get correct id
	// 24401 instead 2441, for example
	if(iNetworkId.Length() < 2)
		{
		compNetworkId.AppendNum(static_cast<TInt64>(0));
		}
	compNetworkId.Append(iNetworkId);
	TLex lexer(compNetworkId);
	lexer.Val(iCompoundNetworkId);
	if(iCompoundNetworkId != 0 && iPreviousCompoundNetworkId != iCompoundNetworkId)
		{
		if(iPreviousCompoundNetworkId != 0) 
			{
			iNetworkChangeObserver.NetworkIdChanged();
			}
		iPreviousCompoundNetworkId = iCompoundNetworkId;
		}
	}


// ---------------------------------------------------------------------------
// Reads network info from Central Repository
// ---------------------------------------------------------------------------
//
#ifdef __WINS__
void CFMRadioMobileNetworkInfoListener::GetCenRepNetworkInfoL()
    {
    TInt length = KFMRadioDefaultNetworkInfoBufferSize;
    HBufC* valueString = HBufC::NewLC( length );
    TInt error( KErrTooBig );

    do
        {
        TPtr ptr = valueString->Des();

        error = iRepository->Get( KFMRadioCREmulatorNetworkInfo, ptr );
        if ( error == KErrTooBig )
            {
            CleanupStack::PopAndDestroy( valueString );
            length = 2 * length;
            valueString = HBufC::NewLC( length );
            }
        } while ( error == KErrTooBig );

    if ( error ) 
        {
        TPtr ptr = valueString->Des();
        ptr.Copy( KNullDesC() );	
        }

    CleanupStack::Pop( valueString );
    delete iEmulatorNetworkInfo;
    iEmulatorNetworkInfo = valueString;
    }
#endif


// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::CompoundNetworkId
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CFMRadioMobileNetworkInfoListener::CompoundNetworkId() const
	{
	return iCompoundNetworkId;
	}

// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::CountryCode
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CFMRadioMobileNetworkInfoListener::CountryCode() const
	{
	return iCountryCode;
	}
	
// ---------------------------------------------------------
// CFMRadioMobileNetworkInfoListener::SubscriberId
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TDesC& CFMRadioMobileNetworkInfoListener::SubscriberId() const
	{
	return *iSubscriberId;
	}

//  End of File  
