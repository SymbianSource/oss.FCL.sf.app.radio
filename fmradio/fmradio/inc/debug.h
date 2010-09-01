/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Central place for nice debug-type macros & functions
*
*
*/


#ifndef FMRADIO_DEBUG_H
#define FMRADIO_DEBUG_H

#ifdef _DEBUG

#ifdef __WINS__
#define __CLOGGING__
// File logging for WIS
//#define __FLOGGING__

#else

// Logging with RDebug for target HW
#define __CLOGGING__
//#define __FLOGGING__

#endif //__WINS__

#endif

#if defined ( __FLOGGING__ )

_LIT(KLogFile,"FMRadioLog.txt");
_LIT(KLogDir,"FMRadio");

#include <f32file.h>
#include <flogger.h>

#define FLOG(a) {FPrint(a);}

#define FLOGHEX(value, len) {RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, "", " ",value, len);}

#define FDEBUGVAR(a) a
#define FTRACE(a) {a;}

#define FDEBUGVAR(a) a

// Declare the FPrint function

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);

}

inline void FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void FHex(const TDesC8& aDes)
{
    FHex(aDes.Ptr(), aDes.Length());
}

// RDebug logging
#elif defined(__CLOGGING__)

#include <e32svr.h>

#define FLOG(a) {RDebug::Print(a);}

#define FLOGHEX(a)

#define FDEBUGVAR(a) a
#define FTRACE(a) {a;}

#define FDEBUGVAR(a) a

// Declare the FPrint function

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    TInt tmpInt = VA_ARG(list, TInt);
    TInt tmpInt2 = VA_ARG(list, TInt);
    TInt tmpInt3 = VA_ARG(list, TInt);
    VA_END(list);
    RDebug::Print(aFmt, tmpInt, tmpInt2, tmpInt3);
}


#else   // No loggings --> reduced code size
#define FLOG(a)
#define FLOGHEX(a)
#define FDEBUGVAR(a)
#define FTRACE(a)
#define FDEBUGVAR(a)

#endif //_DEBUG

#endif // FMRADIO_DEBUG_H
