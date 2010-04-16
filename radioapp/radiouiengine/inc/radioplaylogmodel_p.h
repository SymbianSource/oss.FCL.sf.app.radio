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

#ifndef RADIOPLAYLOGMODEL_P_H
#define RADIOPLAYLOGMODEL_P_H

// System includes
#include <QList>
#include <QString>

// Forward declarations
class RadioPlayLogModel;
class RadioUiEngine;
class RadioPlayLogItem;

class RadioPlayLogModelPrivate
{
public:

    explicit RadioPlayLogModelPrivate( RadioPlayLogModel* model, RadioUiEngine& uiEngine );

    ~RadioPlayLogModelPrivate();

public: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioPlayLogModel*          q_ptr;

    /**
     * Reference to the ui engine
     */
    RadioUiEngine&              mUiEngine;

    /**
     * List of play log items
     */
    QList<RadioPlayLogItem>     mItems;

    QString                     mRtItemHolder;

    bool                        mTopItemIsPlaying;

    bool                        mShowDetails;

};

#endif // RADIOPLAYLOGMODEL_P_H
