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

#ifndef RADIOHISTORYMODEL_P_H
#define RADIOHISTORYMODEL_P_H

// System includes
#include <QList>
#include <QString>

// Forward declarations
class RadioHistoryModel;
class RadioUiEngine;
class RadioHistoryItem;

class RadioHistoryModelPrivate
{
public:

    explicit RadioHistoryModelPrivate( RadioHistoryModel* model, RadioUiEngine& uiEngine );

    ~RadioHistoryModelPrivate();

public: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioHistoryModel*          q_ptr;

    /**
     * Reference to the ui engine
     */
    RadioUiEngine&              mUiEngine;

    /**
     * List of history items
     */
    QList<RadioHistoryItem>     mItems;

    QString                     mRtItemHolder;

    bool                        mTopItemIsPlaying;

    bool                        mShowDetails;

};

#endif // RADIOHISTORYMODEL_P_H
