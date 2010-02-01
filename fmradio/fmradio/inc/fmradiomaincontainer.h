/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the class CFMRadioMainContainer.
*
*/


#ifndef FMRADIOMAINCONTAINER_H
#define FMRADIOMAINCONTAINER_H

// INCLUDES

#include <AknUtils.h>
#include <alf/alfenv.h> 
#include <hwrmlight.h>
#include <hwrmlightdomaincrkeys.h>

#include "fmradio.hrh"
#include "fmradioappui.h"

// FORWARD DECLARATIONS

class CRadioEngine;
class CFMRadioAlfVisualizer;
class CFMRadioAlfMediaIdle;
class CFMRadioAlfIndicator;
class CFMRadioAlfRDSViewer;
class CFMRadioIdleController;
class CFMRadioLogo;
// CLASS DECLARATION
/**
* Interface to tell when timeout event occured
*
*    @since 2.6
*/
class MFMRadioTimer
	{
	public: 
	
		virtual void TimerTimeOut( TInt aErrorStatus ) = 0;
	
	};

/**
* Active object to handle timer timeout events
*
*    @since 2.6
*/	
class CFMRadioTimer : public CActive	
	{
	public: 
		/**
        * Constructor
        * @param aTimerObserver observer for active object
        */
		CFMRadioTimer( MFMRadioTimer* aTimerObserver );
		/**
        * Destructor
        */
        ~CFMRadioTimer();
        /**
        * ActiveObject RunL, which calls observer interface when completes
        */
		void RunL();
		/**
        * ActiveObject DoCancel, which calls observer interface when completes 
        */
		void DoCancel();
		/**
        * Activated active object
        */
        void Activate();
		
	private:
		// Active object observer
		MFMRadioTimer* iTimerObserver;
	
	};
	
	
/**
*  Creates and owns the UI controls related to the main view.
*  
*/
class CFMRadioMainContainer : public CBase,
                      		public MHWRMLightObserver,
							public MFMRadioTimer

    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioMainContainer
        * @since 2.6
        * @param aRect Frame rectangle for container.
        * @param aAlfEnv A reference to the alfred environment
        * @param aRadioEngine A pointer to the radio engine
        */
        static CFMRadioMainContainer* NewL( const TRect& aRect, 
        									CAlfEnv& aAlfEnv,
        									CRadioEngine& aRadioEngine );        
		/**
        * Destructor.
        * @since 2.6
        */
        virtual ~CFMRadioMainContainer();

    public: // New functions
        /**
        * Display the channel information
        * @since 2.6
        * @param aChannelNumber the channel number to display
        * @param aStationName the station name to display
        * @param aChangeType Station change type for the animation
        * @param aFrequency of the channel
        */
        void DisplayChannelInfoL( TInt aChannelNumber, 
                                  const TDesC& aStationName,
                                  TFMRadioStationChangeType aChangeType, 
        						  TInt aFrequency );
        
        /**
         * Formats the frequency number and returns it as a descriptor. 
         * Ownership of the returned string is transfered.
         * @param aFrequency The frequency to be formatted
         * @param aResourceId The resource to be used for formatting the string
         * @return The formatted frequency
         */
        HBufC* FormattedFrequencyStringL( TInt aFrequency, TInt aResourceId );
        
        /**
    	* Dims or undims RDS information
    	* @param   aShow           <code>ETrue</code> if RDS marquee is to be shown, <code>EFalse</code> otherwise.
    	* @param   aFade           <code>ETrue</code> if fade effect it to be used, <code>EFalse</code> otherwise.
    	* @param   aShowAfMarquee  <code>ETrue</code> if AF marquee is also to be shown, <code>EFalse</code> otherwise.
    	*                          This only has effect if aShow is <code>ETrue</code>.
    	*/
    	void ShowRdsInfo( TBool aShow, TBool aFade, TBool aShowAfMarquee = EFalse);

    	/**
    	 * Sets RT+ interaction indicator visible or invisible.
    	 * 
    	 * The indicator is "MusicStore" or "+" depending on KRadioRTPlusSupportLevel cenrep value.
    	 * When it is off, only music store interaction is supported and the indicator is set likewise.
    	 * 
    	 * @param aVisible sets the visibility
    	 * @param aFadeEffect to use transition fade effect or not
    	 */
        void ShowRTPlusInteractionIndicator( TBool aVisible, TBool aFadeEffect);
        
        /**
        * Fades the entire window and controls in the window owned by this container control.
        * @since 2.6
        * @param aFaded true/false to indicate whether the view should fade/unfade
        */
        void SetFaded( TBool aFaded );
        /**
        * Handles resource changes
        * @param aType Resource change type
        */
        void HandleResourceChange(TInt aType);	
        /**
  	    * Sets rectangle of the alfred display
  	    * @param aRect Display rectangle
  	    * @return none
  	    */ 
        void SetRect( const TRect& aRect );           
        /**
        * Returns a pointer to the station information visualizer
        * @return VisualControl
        */
    	CFMRadioAlfVisualizer* VisualControl();
    	/**
    	* Returns a reference to the Media Idle 
    	* @return Media Idle
    	*/
    	CFMRadioAlfMediaIdle&  MediaIdle();
    	
    	/**
    	* Updates RDS text to display  
    	* @return Media Idle
    	*/
    	void SetRdsRadioTextL( const TDesC& aRadioText );
    	 /**
		* From MHWRMLightObserver
		* Lights status has been changed
		*/
		void LightStatusChanged(TInt aTarget, 
                                        CHWRMLight::TLightStatus aStatus);
		/**
        * From MFMTimer To handle timer timeout events.
        */
        void TimerTimeOut( TInt aErrorStatus );

        /**
         * Returns a reference to the RDS Viewer
         * @return RDS Viewer
         */
        CFMRadioAlfRDSViewer& RdsViewer() const;
        /**
         * Returns a reference to the idle controller
         * @return idle controller
         */		
        CFMRadioIdleController& IdleController() const;
        /**
         * Returns a reference to the logo visual
         * @return logo visual 
         */
        CFMRadioLogo& Logo() const;

    private: // New functions
        /**
        * Called when the view size is changed
        */
        void SizeChanged();
		/**
        * Required for help.
		* @since 2.7
		* @param aContext the help context to be used.
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;        	    
    	/**
		* C++ default constructor
        * @param aAlfEnv A reference to the alfred environment
        * @param aRadioEngine A pointer to the radio engine
		*/
		CFMRadioMainContainer( CAlfEnv& aAlfEnv, CRadioEngine& aRadioEngine );
        /**
        * EPOC default constructor
        * @since 2.7
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );
        /**
        * Handles updating main area text colors using current skin
        * @since 3.0
        */
        void UpdateTextColorFromSkin();

    private: //data
        
		TBool iFadeStatus;
		TRect  iMainPaneRect;
		
		 /** Visualizer for the radio information. Not owned. */
		CFMRadioAlfVisualizer* iVisualControl;
		/** Media Idle. Not owned. */
		CFMRadioAlfMediaIdle*  iMediaIdle;
		/** RDS indicator. Not owned. */
	    CFMRadioAlfIndicator*  iRdsIndicator;
		/** RDS AF indicator. Not owned. */
	    CFMRadioAlfIndicator*  iRdsAfIndicator;
		/** RDS viewer. Not owned. */
	    CFMRadioAlfRDSViewer* iRdsViewer;
        /** RDS interaction (music store, web link etc.) indicator. Not owned. */
        CFMRadioAlfIndicator*  iRdsInteractionIndicator;
	    /** A reference to the alfred environment. */
	    CAlfEnv& iAlfEnv;		
		/** A reference to the radio engine */
		CRadioEngine& iRadioEngine;			    
	    /** A pointer to the control environment */
	    CCoeEnv* iCoeEnv;	    
	    /**Light event informer*/
	    CHWRMLight* iLight;
       	/**timer for light dimmed handler*/
       	RTimer timer;
       	/**Active object to handle lights dimmed timer */
       	CFMRadioTimer* iTimer;
       	/**Frame counter */
       	TInt iLightsOffCounter;
        // Media idle controller. Owned
        CFMRadioIdleController* iIdleController;
        // Alf image visual for the default radio icon. Not Owned
        CFMRadioLogo* iRadioLogo;

    };

#endif

// End of File
