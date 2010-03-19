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

#ifndef RADIOPLAYLOGITEM_H
#define RADIOPLAYLOGITEM_H

// System includes
#include <qmetatype>
#include <qobject>

// User includes
#include "radiouiengineexport.h"

// Forward declarations
class RadioPlayLogItemPrivate;

/*!
 * Radio play log item implements a song recognized from the radio broadcast
 *
 * Note! It is derived from QObject even though architecturally it shouldn't be.
 * It is done only to satisfy the WINSCW build which doesn't export the destructor in UREL
 * build. This causes mismatching def files and annoying warnings. Deriving from QObject fixes this
 */
class UI_ENGINE_DLL_EXPORT RadioPlayLogItem : public QObject
{
public:

    RadioPlayLogItem();
    RadioPlayLogItem( const QString& artist, const QString& title );
    RadioPlayLogItem( const RadioPlayLogItem& other );

    ~RadioPlayLogItem();

    RadioPlayLogItem& operator=( const RadioPlayLogItem& other );

    QString artist() const;
    void setArtist( const QString& artist );

    QString title() const;
    void setTitle( const QString& title );

    bool isFavorite() const;
    void setFavorite();

    void increasePlayCount();
    int playCount() const;

private:

    /**
     * Decrements the reference count of the implicitly shared data.
     * Data is deleted if no instance uses it anymore.
     */
    void decrementReferenceCount();

private: // data

    /**
     * Pointer to the implicitly shared private implementation
     * Own.
     */
    class RadioPlayLogItemPrivate* mData;

public:

    /**
     * Detach from the implicitly shared data
     */
    void detach();

    /**
     * Checks if the class is detached from implicitly shared data
     * Required by many QT convenience functions for implicitly shared classes
     */
    bool isDetached() const;

    typedef RadioPlayLogItemPrivate* DataPtr;
    inline DataPtr &data_ptr() { return mData; }

};

Q_DECLARE_TYPEINFO( RadioPlayLogItem, Q_MOVABLE_TYPE );  // Can be moved around in memory by containers if necessary
Q_DECLARE_SHARED( RadioPlayLogItem )                     // Uses implicit sharing
Q_DECLARE_METATYPE( RadioPlayLogItem )                   // To be usable in a QVariant

#endif // RADIOPLAYLOGITEM_H
