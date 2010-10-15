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

#ifndef RADIOTIMERPOOL_H
#define RADIOTIMERPOOL_H

// System includes
#include <QObject>
#include <QVariant>
#include <QScopedPointer>

// User includes

// Forward declarations
class QTimer;

class RadioTimerPool : public QObject
{
    Q_OBJECT

    class RadioTimerItem
    {
    public:

        RadioTimerItem( RadioTimerPool* parent );
        ~RadioTimerItem();
        void reset();

        QScopedPointer<QTimer>  mTimer;
        QObject*                mReceiver;
        QString                 mReceiverMember;
        int                     mId;
        QVariant                mParam;
    };

public:

    RadioTimerPool( QObject* parent = 0 );
    ~RadioTimerPool();

    void startTimer( int msec, int id, QObject* receiver, const char* member, QVariant param );
    void cancelTimer( int id, const QObject* receiver );
    bool isTimerActive( int id, const QObject* receiver ) const;

    void freeUnusedTimers();

private Q_SLOTS:

    void timerFired();

private:

    RadioTimerPool::RadioTimerItem* createItem();

private: // data

    QMap<QTimer*,RadioTimerItem*>   mTimerMap;

};


#endif // RADIOTIMERPOOL_H
