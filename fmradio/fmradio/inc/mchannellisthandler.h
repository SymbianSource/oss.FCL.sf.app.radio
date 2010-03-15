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
* Description:  Callback definitions to channel main list update
*                     
*
*/

#ifndef MCHANNELLISTHANDLER_H

#define MCHANNELLISTHANDLER_H

#include "fmradiopreset.h"

/**
*	Operations to main list
*    @since 2.6
*/
enum TMoveoperations
	{
	EMoveChannels,
	EDeleteChannel,
	EAddChannelToList,
	EStoreAllToRepository,
	EStoreIndexToRepository,
	ERemoveAllFromRepository
	};


/**
* Interface to update channel list
*
*    @since 2.6
*/
class MChannelListHandler
	{
	public:
		
		
		virtual void UpdateChannelsL( TMoveoperations aOperation, 
				TInt aIndex, 
				TInt aMovedToNewIndex ) = NULL;
		
		virtual void AddChannelToListL( const TDesC& aChannelName, TInt aChannelFreq ) = 0;
		
		virtual RPointerArray<CFMRadioPreset>& Channels() = 0;
		
	};

#endif // MCHANNELLISTHANDLER_H
