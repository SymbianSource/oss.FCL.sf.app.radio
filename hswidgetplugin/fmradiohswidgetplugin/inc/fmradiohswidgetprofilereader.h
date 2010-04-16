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
* Description: FM Radio widget profile reader
*
*/

#ifndef FMRADIOHSWIDGETPROFILEREADER_H_
#define FMRADIOHSWIDGETPROFILEREADER_H_

// System includes
#include <QObject>
#include <QVariant>

// Forward declarations
class XQSettingsManager;
class XQSettingsKey;

class FmRadioHsWidgetProfileReader : public QObject
{
Q_OBJECT

public:
    FmRadioHsWidgetProfileReader(QObject *parent = 0);
    virtual ~FmRadioHsWidgetProfileReader();

signals:
    void profileChanged(int profile);

public slots:
    void itemDeleted(const XQSettingsKey& key);
    void handleChanges(const XQSettingsKey& key, const QVariant& value);
    
private slots:
    void currentProfileStatus(QVariant value);
    
private: // data
    XQSettingsManager *mSettingsManager; // For getting and P&S notifications

};

#endif /* FMRADIOHSWIDGETPROFILEREADER_H_ */
