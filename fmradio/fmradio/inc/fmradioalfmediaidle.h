/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the class CFMRadioAlfMediaIdle.
*
*/


#ifndef FMRADIOALFMEDIAIDLE_H_
#define FMRADIOALFMEDIAIDLE_H_

// INCLUDES

#include <AknUtils.h>
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>

#include "fmradioidlecontrolinterface.h"

// FORWARD DECLARATIONS

class CAlfGradientBrush;
class CAlfEnv;
class CAlfVisual;

// CONSTANTS

// CLASS DEFINITIONS

/**
* CFMRadioAlfMediaIdle class
* 
* Class launches animated information in the defined subregion of the display. 
*/ 
NONSHARABLE_CLASS(CFMRadioAlfMediaIdle) : public CAlfControl,
                                          public MFMRadioIdleControlInterface
	{
public:		// Constructors and destructor

		/** Display orientations. */
		enum TOrientation
			{
			EPortrait = 0x4A, 
			ELandscape,
			EOrientationNone
			};
			
		/**
	    * Epoc two-phased constructor.
	    * @param aEnv Reference to Alfred environment instance.
        * @return Pointer to this media idle component.
	    */
		static CFMRadioAlfMediaIdle* NewL( CAlfEnv& aEnv );
		/**
        * Destructor
        */
		virtual ~CFMRadioAlfMediaIdle();
public: 	// Functions from base classes		
		/**
 	    * From CAlfControl, takes care of alfred event handling.
 	    */
		TBool OfferEventL( const TAlfEvent& aEvent );	
		
public:		// New functions
		
		/** Media idle modes. */
		enum TMediaIdleMode
			{
			EOnlyBackgroundText = 0xB0, 
			EBackgroundTextAndMediaIdleItems
			};
				
		/**
 	    * Adds media idle item text to be used in the media idle
 	    * @param aMediaIdleItemText Text of the media idle item
 	    */								   
		void AddMediaIdleContentL( const TDesC& aMediaIdleItemText );
		/**
	    * Removes all media idle item texts
		*/			
		void ResetMediaIdleContent();
		/**
	    * Returns number of media idle content items
	    * @return content count
		*/	
		TInt MediaIdleContentCount() const;
		/**
	    * Sets the mode request for mode change. The mode 
	    * will be changed as soon as possible. 
	    * @param aRequestedMode Requested mode
		*/
		void SetModeChangeRequestL( TMediaIdleMode aRequestedMode );
		/**
	    * Retrieves the media idle mode
		*/
		CFMRadioAlfMediaIdle::TMediaIdleMode Mode();
		/**
 	    * Sets primary color for the media idle
 	    * @param aColor The color that is used as primary color of media idle
 	    */
		void SetPrimaryColor( const TRgb& aColor );
		/**
 	    * Sets secondary color for the media idle
 	    * @param aColor The color that is used as secondary color of media idle
 	    */
		void SetSecondaryColor( const TRgb& aColor ); 

		/**
 	    * Return orientation of the display
 	    */
		CFMRadioAlfMediaIdle::TOrientation Orientation();
		/**
 	    * Set orientation of the display
 	    * @param aOrientation The display orientation
 	    */
		void SetOrientation( TOrientation aOrientation );		
		/** 
		* Calculates display layout
	    */			
		void UpdateLayout();
		/** 
		* Adds PS name to the media idle item array
	    */	
        void AddPsNameToMediaIdleL( const TDesC& aPsName );

private:
		/**
		* C++ default constructor
		*/
		CFMRadioAlfMediaIdle( );
		/**
	    * EPOC second phase constructor.
	    * @param aEnv Reference to the Hitchcock environment instance.
	    */
		void ConstructL( CAlfEnv& aEnv );
		
private:	// New functions

		/** Scroll directions for the media idle item. */
		enum TMediaIdleItemScrollDirection
			{	
			EScrollLeftToRight = 0xA0,
			EScrollRightToLeft,
			EScrollNone
			};
			
        /** States of media idle. */
        enum TMediaIdleState
            {
            ERunning = 0xA0,
            EStopped
            };
            
        /** Custom events for the media idle. */
        enum TMediaIdleCustomEvent
            {
            EMediaIdleItemTransformationEnd = 0x5B,
            EMediaIdleBackgroundTextScrollEnd,
            EMediaIdleFadeOutAnimationEnd,
            EMediaIdleChangeMode,
            EMediaIdleEventNone
            };
        
        /**
        * Launches media idle immediately.
        */	
        void StartMediaIdleL();	
		/**
 	    * Creates media idle with passed number of the media idle items
 	    * @param aNumberOfMediaIdleItems Number of the media idle items
 	    */
		void AddMediaIdleL( const TInt aNumberOfMediaIdleItems );
		/**
 	    * Creates the media idle item to the passed media idle view port.
 	    * @param aMediaIdle The view port of the media idle
 	    * @param aMediaIdleItemText Text of the media idle item
 	    * @param aMediaIdleItemHasBackground TBool to indicate background visibility
 	    * @param aBackgroundColor The background color of the media idle item
 	    * @param aBackgroundOpacity The background opacity of the media idle item
 	    */
		CAlfTextVisual* AddMediaIdleItemL( CAlfLayout* aMediaIdle, 
										   const TDesC& aMediaIdleItemText, 
									       TBool aMediaIdleItemHasBackground = EFalse, 
										   const TRgb& aBackgroundColor = KRgbWhite, 
										   const TReal aBackgroundOpacity = 0.5f );
		/**
 	    * Initializes the media idle items
 	    * @param aNumberOfMediaIdleItems Number of the media idle items
 	    */
		void InitializeMediaIdleItemsL( const TInt aNumberOfMediaIdleItems );
		/**
 	    * Sets defined animation to the created media idle items
 	    */
		void AnimateMediaIdleItems();		
		/**
 	    * Retrieves text from randomly chousen media idle item
 	    */	
		HBufC* GetRandomMediaIdleContentL();
		/**
 	    * Sets color, opacity and scaling settings to the media idle item
 	    * @param aMediaIdleItem The media idle item 
 	    * @param aMediaIdleItemText Text of the media idle item
 	    * @param aTextStyleId Text style for text of the media idle item
 	    */		   
		void SetMediaIdleItemSettingsL( CAlfTextVisual* aMediaIdleItem, const TDesC& aMediaIdleItemText, const TInt aTextStyleId );
		/**
 	    * Sets translating animations to the media idle item
 	    * @param aMediaIdleItem The media idle item 
 	    */
		void SetMediaIdleItemAnimation( CAlfTextVisual* aMediaIdleItem );
		/**
	    * Sets the media idle mode
		*/
		void SetMode( TMediaIdleMode aMode );
		/**
	    * Sets image to display
	    * @param aBitmapFileName Full path to image file
	    * @param aAbsoluteRect   Rect of the image
	    * @param aBorders 		 
	    * @param aParentLayout   
	    */
		void SetImageByAbsoluteRectL( const TDesC& aBitmapFileName,
									  const TRect& aAbsoluteRect,
									  TBool aBorders,
									  CAlfLayout* aParentLayout = 0 );		
		/**
	    * Fades in media idle
	    * @param aNow If ETrue shows media idle immediately otherwise
	    * fades with defined fading duration
		*/
		void MediaIdleFadeIn( TBool aNow = EFalse );
		/**
	    * Fades out media idle
	    * @param aNow If ETrue hides media idle immediately otherwise
	    * fades with defined fading duration
		*/
		void MediaIdleFadeOut( TBool aNow = EFalse );
		/**
 	    * Sets and starts scrolling animation to CAlfTextVisual.
 	    * The visual object is scrolled from current position to 
 	    * right side of the display
 	    * @param aVisual a pointer to the visual object
 	    */
		void ScrollToRight( CAlfTextVisual* aVisual ) const;
		/**
 	    * Sets and starts scrolling animation to CAlfTextVisual.
 	    * The visual object is scrolled from current position to 
 	    * left side of the display
 	    * @param aVisual a pointer to the visual object
 	    */
		void ScrollToLeft( CAlfTextVisual* aVisual ) const;
		/**
 	    * @param aVisual a pointer to the visual object
 	    * @param aTransitionTime Duration for reaching the target.
 	    * @param aEventAfterScrollingEnd custom even that will be sent when transition time is end
 	    */
		void SetScrollingToBackgroundText( CAlfTextVisual* aVisual,
							    TInt aTransitionTime,
							    TMediaIdleCustomEvent aEventAfterScrollingEnd = EMediaIdleEventNone ) ;		
		/**
	    * Sets fade-in animation to the CAlfVisual.
	    * @param aVisual a pointer to the visual object
	    * @param aFadingTime Time duration after the visual object has been faded in.
	    *                    Fading duration in milliseconds 
	    * @param aOpacity Target opacity value
	    */
		void FadeIn( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity = 1.0f ) const;
		/**
	    * Sets fade-out animation to the CAlfVisual.
	    * @param aVisual a pointer to the visual object
	    * @param aFadingTime Time duration after the visual object has been faded out.
	    *                    Fading duration in milliseconds 
	    * @param aOpacity Target opacity value
	    */
		void FadeOut( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity = 0.0f ) const;
		/**
 	    * Translates the visual object with passed arguments
 	    * @param aTextVisual a pointer to the visual object 
 	    * @param aX Translation value in the horizontal direction
 	    * @param aY Translation value in the vertical direction
 	    */
		void Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, const TAlfTimedValue& aY );		
		/**
 	    * Removes all transformations of the visual object such as the scaling and translating.
 	    * @param aTextVisual a pointer to the visual object 
 	    */
		void LoadTextVisualIdentity( CAlfTextVisual* aTextVisual );
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
        /**
	    * Sets absolute rect of the anchor by top left and size of the rect.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aSize Size of the rect
	    */	
		void SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
									   TInt aOrdinal,
									   const TPoint& aTopLeftPosition,
									   const TSize& aSize );
		/**
	    * Sets relative rect of the anchor by top left and bottom right points.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aBottomRightPosition Bottom right point of the rect
	    */
		void SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
									   TInt aOrdinal,
									   const TAlfRealPoint& aTopLeftPosition,
									   const TAlfRealPoint& aBottomRightPosition );
		/**
	    * Sets relative rect of the anchor by top left and size of the rect.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aSize Size of the rect
	    */
		void SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor, 
									   TInt aOrdinal, 
									   const TAlfRealPoint& aTopLeftPosition, 
									   const TAlfRealSize& aSize );	
		       

        
        /*
         * Transformation end event received
         */
		void HandleMediaIdleItemTransformationEndEventL( const TAlfEvent& aEvent );
        
		/**
		 * Background text scroll end event received
		 */
        void HandleBackgroundTextScrollEndEventL( const TAlfEvent& aEvent );
        
        /**
         ** Create fonts for media idle; based on a list of relative font sizes
         */
        void CreateFontsL();
        
        /**
         * Create a background idle text visual
         */
        void CreateBackgroundTextL();
        /**
         * Stops media idle
         */
        void StopAndFadeOutMediaIdle();
        
        // from MFMRadioIdleControlInterface
        void Show();
        
        /**
        * from MFMRadioIdleControlInterface
        */            
        void Hide();
        
        /**
        * from MFMRadioIdleControlInterface
        */        
        void Deactivate();

private:	// Data 	    
        /** Status flag for knowing should the fonts be recreated or not */
        TOrientation iFontsOrientation;
		/** Display orientation */
	    TOrientation iOrientation;

	    /** Layout for media idle */
	    TAknLayoutRect iMediaIdleLayout; 
	    /** Text style id of the media idle background text */
        TInt iMediaIdleItemBackgroundTextStyleId;
        /** States of media idle */
		TMediaIdleState iMediaIdleState;
		/** Keeps track of the current media idle mode */
		TMediaIdleMode  iMediaIdleMode;
		/** Keeps track of the requested mode */
		TMediaIdleMode  iMediaIdleRequestedMode;
	    /** Counts the items that have finished their transformations for mode change */
		TInt iItemsTransformationEndEventCounter;
		
	    /** Color to be used for the media idle items */
	    TRgb iMediaIdlePrimaryColor;
	    /** Color to be used for the media idle items */
	    TRgb iMediaIdleSecondaryColor;
	    /** Scroll direction for the media idle item */
	    TMediaIdleItemScrollDirection iScrollDirection;
	    	
		/** Array for the strings of the media idle items */
        CDesCArrayFlat* iMediaIdleItemArray;
		/** Array for the text styles of the media idle items */
        RArray<TInt> iTextStyleIdArray;
	    /** Scroll direction for the media idle background text */
	    TMediaIdleItemScrollDirection iBackgroundTextScrollDirection;		 	   
	    /** Indicates whether the instance is fully constructed */  
	    TBool iIsConstructed;
	    /** the latest PS name added to the media idle item array. Owned */
	    HBufC* iPsName;
	    /** The last size for font creation */
	    TInt iFontReferenceHeight;
	};

#endif /*FMRADIOALFMEDIAIDLE_H_*/
