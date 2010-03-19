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

#ifndef RADIOBANNERLABEL_H_
#define RADIOBANNERLABEL_H_

// System includes
#include <hblabel.h>

class RadioBannerLabel : public HbLabel
{
    Q_OBJECT

public:

    RadioBannerLabel( QGraphicsItem * parent = 0 );

private:

// from base class QGraphicsItem

    void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

};

#endif // RADIOBANNERLABEL_H_
