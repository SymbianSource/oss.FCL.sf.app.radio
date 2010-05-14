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

#ifndef RADIOCAROUSELMODEL_P_H
#define RADIOCAROUSELMODEL_P_H

// System includes

// Forward declarations
class RadioCarouselModel;
class RadioUiEngine;
class RadioStationModel;

class RadioCarouselModelPrivate
{
public:

    explicit RadioCarouselModelPrivate( RadioCarouselModel* model,
                                        RadioUiEngine& uiEngine,
                                        RadioStationModel& stationModel );

    ~RadioCarouselModelPrivate();

public: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioCarouselModel*          q_ptr;

    /**
     * Reference to the ui engine
     */
    RadioUiEngine&              mUiEngine;

    RadioStationModel&          mStationModel;

};

#endif // RADIOCAROUSELMODEL_P_H
