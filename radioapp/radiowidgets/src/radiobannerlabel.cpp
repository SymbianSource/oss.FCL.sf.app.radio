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

#include "radiobannerlabel.h"

#include <QPainter>

/*!
 *
 */
RadioBannerLabel::RadioBannerLabel( QGraphicsItem * parent ) :
    HbLabel( parent )
{
    QFont currentfont( font() );
    currentfont.setBold( true );
    currentfont.setPixelSize( 18 );
    setFont( currentfont );
}

/*!
 *
 */
void RadioBannerLabel::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    painter->setPen(QPen(QColor(200, 200, 200, 150)));
    painter->setBrush(QBrush(QColor(200, 200, 200, 150),Qt::SolidPattern));
    painter->drawRect(option->rect);
    HbLabel::paint( painter, option, widget );
}
