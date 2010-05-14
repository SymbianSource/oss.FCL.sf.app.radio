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
* Description: FM Radio widget process handler
*
*/

// System includes
#include <ProfileEngineSDKCRKeys.h>
#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "xqpublishandsubscribeutils.h"
#include <QDateTime>

// User includes
#include "fmradiohswidgetprofilereader.h"
#include "fmradiohswidget.h"
#include "radioservicedef.h"

/*!
 Constructor
 */
FmRadioHsWidgetProfileReader::FmRadioHsWidgetProfileReader(QObject *parent) :
    QObject(parent),
    mSettingsManager(new XQSettingsManager(this)),
    mRadioStatus(-1)
{
/*
    // Monitors devices profile.
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
        KCRUidProfileEngine.iUid, KProEngActiveProfile);
    mSettingsManager->startMonitoring(profileKey);
    currentProfileStatus(mSettingsManager->readItemValue(profileKey));
    
    connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)), this,
        SLOT(itemDeleted(XQSettingsKey)));
    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleChanges(XQSettingsKey, QVariant)));
*/
    startMonitoringRadioRunningStatus();
    bool d = connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)), this,
        SLOT(itemDeleted(XQSettingsKey)));
    bool h = connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleRadioRunningChanges(XQSettingsKey, QVariant)));
}

/*!
 Destructor 
 */
FmRadioHsWidgetProfileReader::~FmRadioHsWidgetProfileReader()
{
}


/*!
 Handling of deletion of listened P&S key.
 
 \param key Deleted key.
 */
void FmRadioHsWidgetProfileReader::itemDeleted(const XQSettingsKey& key)
{
/*
    if (key.uid() == KCRUidProfileEngine.iUid && key.key()
        == KProEngActiveProfile) {
    }
*/
    if (key.uid() == KRadioPSUid && key.key() == KRadioStartupKey) {
        startMonitoringRadioRunningStatus();
    }
}

/*!
 Notifications from settings manager are handled and routed to appropriate
 private slots.

 \param key Changed key.
 \param value Value of changed key.
 */
/*
void FmRadioHsWidgetProfileReader::handleChanges(const XQSettingsKey& key,
    const QVariant& value)
{ 
    if (key.uid() == KCRUidProfileEngine.iUid && key.key()
        == KProEngActiveProfile) {
        currentProfileStatus(value);
    }
}
*/

/*!
 Handling changes in profile information.
 
 \param value Originally information is of int type. Valid values after
 conversion are described by KProEngActiveProfile in ProfileEngineSDKCRKeys.h.
 */
void FmRadioHsWidgetProfileReader::currentProfileStatus(QVariant value)
{
    if (value.canConvert(QVariant::Int)) {
        emit profileChanged(value.toInt());
    }
}

/*!
 Notifications from settings manager are handled and routed to appropriate
 private slots.

 \param key Changed key.
 \param value Value of changed key.
 */
void FmRadioHsWidgetProfileReader::handleRadioRunningChanges(const XQSettingsKey& key,
    const QVariant& value)
{
    if (key.uid() == KRadioPSUid && key.key()
        == KRadioStartupKey) {
        currentRadioRunningStatus(value);
    }
}

/*!
 Handling changes in profile information.
 
 \param value Originally information is of int type. Valid values after
 conversion are described by KProEngActiveProfile in ProfileEngineSDKCRKeys.h.
 */
void FmRadioHsWidgetProfileReader::currentRadioRunningStatus(QVariant value)
{
    if (value.isValid()) {
        if (value.canConvert(QVariant::Int)) {
            mRadioStatus = value.toInt();
            QVariant state(FmRadioHsWidget::Running);
            emit radioRunning(state);
        }
    } else {
        mRadioStatus = -1;
        QVariant state(FmRadioHsWidget::NotRunning);
        emit radioRunning(state);
    }
}

QVariant FmRadioHsWidgetProfileReader::radioStatus()
{
    QVariant state;
    if (mRadioStatus != -1) {
        state = QVariant(FmRadioHsWidget::Running);
        return state;
    } else {
        state = QVariant(FmRadioHsWidget::NotRunning);
        return state;
    }
}

void FmRadioHsWidgetProfileReader::startMonitoringRadioRunningStatus()
{
    XQSettingsKey radioRunningKey(XQSettingsKey::TargetPublishAndSubscribe, KRadioPSUid,
        KRadioStartupKey);
    bool a = mSettingsManager->startMonitoring(radioRunningKey);
    currentRadioRunningStatus(mSettingsManager->readItemValue(radioRunningKey));
}
