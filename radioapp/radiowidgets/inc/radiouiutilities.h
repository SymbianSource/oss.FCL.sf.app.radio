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

#ifndef _RADIOUIUTILITIES_H_
#define _RADIOUIUTILITIES_H_

// System includes
#include <QPointer>

// Forward declarations
class RadioFrequencyStrip;
class RadioStationCarousel;

struct EffectInfo
{
    EffectInfo( QGraphicsItem* item, QString path, QString event ) :
        mItem( item ),
        mPath( path ),
        mEvent( event )
    {}

    QGraphicsItem* mItem;
    QString mPath;
    QString mEvent;

private:
    EffectInfo();
};
typedef QList<EffectInfo> QEffectList;

// Class declaration
class RadioUiUtilities
{
public:

    static bool addEffects( QEffectList list );

    static RadioFrequencyStrip* frequencyStrip();
    static RadioStationCarousel* carousel();

    static void setFrequencyStrip( RadioFrequencyStrip* frequencyStrip );
    static void setCarousel( RadioStationCarousel* carousel );

private:

    RadioUiUtilities();
    ~RadioUiUtilities();

    static RadioUiUtilities& instance();

private: // data

    QPointer<RadioFrequencyStrip> mFrequencyStrip;

    QPointer<RadioStationCarousel> mCarousel;

};


#endif // _RADIOUIUTILITIES_H_
