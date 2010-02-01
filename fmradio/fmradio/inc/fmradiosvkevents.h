/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*  Class which implements handling Side Volume Key events (SVK).
*  This class implements methods of MRemConCoreApiTargetObserver
*
*
*/


#ifndef FMRADIOSVKEVENTS_H
#define FMRADIOSVKEVENTS_H

// INCLUDES
#include <remconcoreapitargetobserver.h>
#include <RemConCallHandlingTargetObserver.h>
#include <e32debug.h>

#include "fmradio.hrh"

// FORWARD DECLARATIONS

// CONSTANTS

enum TAccessoryEvent
	{
	EForward = 1,
	ERewind,
	EPausePlay,
	EStop
	};

// CLASS DECLARATION
class CRemConCallHandlingTarget;
class CRemConCoreApiTarget;
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;


class MFMRadioSvkEventsObserver
    {

	public:



	public:
		/**
		* Called when volume level should be changed.
		* @param aVolumeChange: +1 change volume up
		*                       -1 change volume down
		*/
		virtual void FMRadioSvkChangeVolumeL( TInt aVolumeChange ) = 0;

		/**
		* Called when volume was changed last time 2 seconds ago or
		* timer was started 2 seconds ago. The remove control timer
		* is started when ChangeVolumeL() is called
		* or when started through CFMRadioSvkEvents::StartRemoveTimerL()
		*/
		virtual void FMRadioSvkRemoveVolumeL( ) = 0;

		/**
		* Called when accessory key is pressed.
		* @param aEvent The headset event generated.
		*/
		virtual void FMRadioHeadsetEvent(TAccessoryEvent aEvent) = 0;

    };


class CFMRadioSvkEvents : public CBase,
						  public MRemConCoreApiTargetObserver,
						  public MRemConCallHandlingTargetObserver
    {
    public: // Constructors and destructor
        /**
        * Symbian constructor
        * @param aObserver observer to implement callback functions
        */

        static CFMRadioSvkEvents* NewL( MFMRadioSvkEventsObserver& aObserver );

        /**
        * Starts remove timer. Cancels running remove timer.
        * If SVK event occurs, the timer is cancelled and restarted.
        */
        void StartRemoveTimerL( );

        /**
        * Cancels remove timer.
        * If SVK event occurs, the timer is restarted.
        */
        void CancelRemoveTimer( );

        /**
        * Destructor.
        */
        virtual ~CFMRadioSvkEvents();


	public: // From MRemConCallHandlingTargetObserver

       	void AnswerCall();

        void AnswerEndCall();

        void DialCall( const TDesC8& aTelNumber );

        void EndCall();

        void GenerateDTMF( const TChar aChar );

        void LastNumberRedial();

        void MultipartyCalling( const TDesC8& aData );

        void SpeedDial( const TInt aIndex );

        void VoiceDial( const TBool aActivate );

    protected: // New functions
        /**
        * Default constructor.
        */
        CFMRadioSvkEvents( MFMRadioSvkEventsObserver& aObserver );

        /**
        * EPOC constructor.
        */
        void ConstructL();

    	// From MRemConCoreApiTargetObserver
    	void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
    	 					TRemConCoreApiButtonAction aButtonAct);


    private:

        /**
        * Calls respective observer function
        */
        void DoChangeVolume( );

        /**
        * Calls respective observer function
        */
        void RemoveControlL( );


        /**
        * ChangeVolume is the callback function called from change volume timer.
        */
        static TInt ChangeVolume(TAny* aThis);

        /**
        * RemoveControl is the callback function called from remove control timer.
        */
        static TInt RemoveControl(TAny* aThis);
        
        /**
        * TuneChannel can be used to tune up/down
        */
        void TuneChannel( TFMRadioCommandIds aDirection );

        /**
         * Takes care of remcon volume keys
         * @param aButtonAct the button event  
         * @param aDirection volume up or down
         */
        void HandleVolumeButtons( TRemConCoreApiButtonAction aButtonAct, TInt aDirection );
        

	private:
	    MFMRadioSvkEventsObserver&  iObserver;
	    CRemConInterfaceSelector*   iInterfaceSelector;
	    CRemConCoreApiTarget*       iCoreTarget;
	    CRemConCallHandlingTarget*  iCallTarget;
	    CPeriodic*                  iVolumeTimer;
        CPeriodic*                  iRemoveTimer;

        /// indicates size of volume level change
        TInt                        iChange;
        TBool 				    	iTargetOpen;
    };


#endif

// End of File
