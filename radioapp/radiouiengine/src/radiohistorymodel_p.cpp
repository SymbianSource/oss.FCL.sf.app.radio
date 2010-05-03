/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "radiohistorymodel_p.h"
#include "radiohistoryitem.h"

/*!
 *
 */
RadioHistoryModelPrivate::RadioHistoryModelPrivate( RadioHistoryModel* model,
                                                    RadioUiEngine& uiEngine ) :
    q_ptr( model ),
    mUiEngine( uiEngine ),
    mTopItemIsPlaying( false ),
    mShowDetails( false )
{

}

/*!
 *
 */
RadioHistoryModelPrivate::~RadioHistoryModelPrivate()
{

}
