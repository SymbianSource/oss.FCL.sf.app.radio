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

// Forward declarations

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

private:

    RadioUiUtilities();
    ~RadioUiUtilities();

};


#endif // _RADIOUIUTILITIES_H_
