@REM
@REM Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
@REM All rights reserved.
@REM This component and the accompanying materials are made available
@REM under the terms of "Eclipse Public License v1.0"
@REM which accompanies this distribution, and is available
@REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
@REM
@REM Initial Contributors:
@REM Nokia Corporation - initial contribution.
@REM
@REM Contributors:
@REM
@REM Description:  Wins Command file for Utils Test Module
@REM
@REM
@echo Initializing test environment for utilstestmodule...
copy ..\conf\UtilsTestModule.cfg \epoc32\wins\c\TestFramework\UtilsTestModule.cfg
copy ..\conf\UtilsTestModule.cfg \epoc32\winscw\c\TestFramework\UtilsTestModule.cfg
copy ..\conf\UtilsTestModule.cfg \epoc32\data\z\system\data\UtilsTestModule.cfg
copy ..\init\TestFramework.ini \epoc32\wins\c\TestFramework\TestFramework.ini
copy ..\init\TestFramework.ini \epoc32\winscw\c\TestFramework\TestFramework.ini
copy ..\init\TestFramework.ini \epoc32\data\z\system\data\TestFramework.ini
@echo Test environment initialization for UtilsTestModule... Finished.