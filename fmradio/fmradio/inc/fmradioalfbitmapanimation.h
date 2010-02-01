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
* Description:  Tuning animation for RadioGaGa
*
*/


#ifndef CMFRADIOALFBITMAPANIMATION_H
#define CMFRADIOALFBITMAPANIMATION_H


// INCLUDES

#include <alf/alfcontrol.h>


// FORWARD DECLARATIONS

class CAlfImageVisual;
class CAlfAnchorLayout;


// CONSTANTS


/**
 * Alf control to show tuning animation
 */
class CFMRadioAlfBitmapAnimation : public CAlfControl
    {

	public:			
	    /**
	     * Two-phased constructor.
	     * @param aEnv Alf environment
	    */
	    static CFMRadioAlfBitmapAnimation* NewL( CAlfEnv& aEnv );	    
	    /**
	     * Destructor.
	     */
	    virtual ~CFMRadioAlfBitmapAnimation();
			
	public: // from base class 
		/**
 	     * From CAlfControl, takes care of alfred event handling.
 	     */
		TBool OfferEventL( const TAlfEvent& aEvent );	

	public: // New functions
				/**
		* Shows the indicator with the defined opacity value.
		* @param aShowWithFading ETrue if the indicator will be shown with fading; EFalse otherwise.
		* @see SetOpacityInVisibleState()
		*/
		void Show( TBool aShowWithFading );
		/**
		* Hides the indicator with the defined opacity value.
		* @param aHideWithFading ETrue if the indicator will be hidden with fading; EFalse otherwise. 
		* @see SetOpacityInHiddenState()
		*/
		void Hide( TBool aHideWithFading );								
		/** 
		* Sets the indicator rectangle
		* @param aRect Rectangle to the indicator
	    */	
		void SetRect( const TRect& aRect );
		/**
		* Starts bitmap animation after delay and
		* fades it in while animation is running
		*/
		void StartBmpAnimation();
		/**
		* Stops bitmap animation after delay and
		* fades it out while animation is running		
		*/
		void StopBmpAnimation();

	private:
		// Custom events for the bitmap animation. 
		enum TBmpAnimationCustomEvent
			{
			EBmpAnimationStart,
			EBmpAnimationContinue,
			EBmpAnimationStop
			};
				
		enum TBmpAnimationRunningState
		    {
		    EBmpAnimationStopped,
		    EBmpAnimationStartup,
		    EBmpAnimationRunning
		    };
		
		void StartAnimation();

	    CFMRadioAlfBitmapAnimation();

	    void ConstructL( CAlfEnv& aEnv );
	
		void CreateImageAnimationVisualL();
		
		void CreateBitmapAnimationArrayL();	
						
		void ContinueBmpAnimation();
		
		/**
	    * Sets absolute rect of the anchor by top left and bottom right points.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aBottomRightPosition Bottom right point of the rect
	    */
		void SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
									   TInt aOrdinal,
								       const TPoint& aTopLeftPosition,
									   const TPoint& aBottomRightPosition );

	private: // data

		// Visual containing current frame
		CAlfImageVisual* iAnimationImageVisual;
		// Animation frame array
		RArray<TAlfImage> iFrames;
		// Interval to change frames
		TInt iFrameInterval;
		// mbm file for bitmaps
		HBufC* iBitmapFileName;			    	 
	    /** Anchor layout for Bmpanimation */
	    CAlfAnchorLayout* iBmpAnimationAnchor;
	    TInt iCurrentFrameIndex;
	   	/** Bmpanimation opacity in visible state */
	    TReal iOpacityInVisibleState;
	    /** Bmpanimation opacity in hidden state */
	    TReal iOpacityInHiddenState;
	   	/** Bmpanimation rectangle */
	    TRect iRect;
	    /** Animation state */
	    TBmpAnimationRunningState iAnimationState;
	    
	    	
    };


#endif // CMFRADIOALFBITMAPANIMATION_H
