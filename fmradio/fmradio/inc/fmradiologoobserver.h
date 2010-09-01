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
* Description:  Declares an interface for observing radio logo event
*
*/

#ifndef FMRADIOALFLOGOOBSERVER_H_
#define FMRADIOALFLOGOOBSERVER_H_

/*
 * Interface for observing radio logo event
 */
class MFMRadioLogoObserver
    {
    public:
        /*
         * Logo visibility time has ended
         */
        virtual void LogoDisplayTimeCompleted() = 0;
    };
#endif /* FMRADIOALFLOGOOBSERVER_H_ */
