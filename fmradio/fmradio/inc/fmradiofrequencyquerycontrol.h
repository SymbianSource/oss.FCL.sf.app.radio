/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioFrequencyQueryControl
*
*/


#ifndef FMRADIOFREQUENCYQUERYCONTROL_H
#define FMRADIOFREQUENCYQUERYCONTROL_H

#include	<aknEditStateIndicator.h>
#include	<AknIndicatorContainer.h>
#include	<aknQueryControl.h>

// CONSTANTS

const TInt KFMRadioFrequencyQueryDialogCustomAdditionHeight = 110;

class CFMRadioFrequencyEditor;
class CAknsFrameBackgroundControlContext;

/**
 * Custom query control for frequency query dialog.
 */
class CFMRadioFrequencyQueryControl : public CAknQueryControl
    {

public:  // Methods

    /**
     * C++ default constructor.
     */
    CFMRadioFrequencyQueryControl();
    
    /**
     * Destructor.
     */
    ~CFMRadioFrequencyQueryControl();

	/**
     * This control is constructed by the system using this static method.
     */
	static SEikControlInfo StaticCreateCustomControl( TInt aIdentifier );

    /**
	 * Gets the frequency from the control
	 *
	 * @return Frequency
     */
    TUint32 Frequency() const;

	/**
	 * Sets the frequency to the control
	 *
	 * @param aFreq Frequency
     */
    void SetFrequency(const TUint32 aFreq);

	/**
	 * Handle possible editor leave.
	 *
	 * @return ETrue, if editor can be left.
	 */
	TBool CanLeaveEditorL();

    // from base class CAknQueryControl
	/**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::ControlByLayoutOrNull( TInt aLayout )
     */
    CCoeControl* ControlByLayoutOrNull( TInt aLayout );

	/**
     * From CAknQueryControl
     * 
     * @see CAknQueryControl::EditorContentIsValidL() const
     */
    TBool EditorContentIsValidL() const;

	/**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::SetAndUseFlags( TBitFlags16 aFlags )
     */
    void SetAndUseFlags( TBitFlags16 aFlags );

	/**
	 * From CAknQueryControl
	 *
	 * @see CAknQueryControl::WindowLayout( TAknWindowLineLayout& aLayout ) 
	 *  const
	 */
	void WindowLayout( TAknWindowLineLayout& aLayout ) const;


    // from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see CCoeControl::PrepareForFocusLossL()
     */
    void PrepareForFocusLossL();

	/**
     * From CCoeControl
     *
     * @see CCoeControl::FocusChanged( TDrawNow aDrawNow )
     */
    void FocusChanged( TDrawNow aDrawNow );

	/**
     * From CCoeControl
     *
     * @see CCoeControl::CountComponentControls() const
     */
    TInt CountComponentControls() const;

	/**
     * From CCoeControl
     *
     * @see CCoeControl::ComponentControl( TInt aIndex ) const
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
    /**
     * Is public so that callbacks may utilize this.
     * Sets repeat intervals.
     * 
     * @param aAccelerated ETrue=accelerated speed, EFalse=default values
     */
    void SetAcceleratedLongPress(TBool aAccelerated);
    
    /**
     * From CCoeControl
     * 
     * @see CCoeControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
     * */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
    // from base class MCoeControlObserver
	/**
     * From MCoeControlObserver
     *
     * @see MCoeControlObserver::HandleControlEventL( 
     *    CCoeControl* aControl, 
     *    TCoeEvent aEventType )
     */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

	class CFMRadioManualTuningEditorIndicator : public CAknIndicatorContainer, public MAknEditingStateIndicator
		{
		public:
			
			static CFMRadioManualTuningEditorIndicator* NewL(CCoeControl* aControl);
			
			~CFMRadioManualTuningEditorIndicator();

			/**
			 * From MAknEditingStateIndicator
			 */
			void SetState(TAknEditingState aState); 

			/**
			 * From MAknEditingStateIndicator
			 */
			CAknIndicatorContainer* IndicatorContainer();

			/**
			 * Sets the information about the query flags.
			 */
			void SetFlags(TBitFlags16 aFlags);

		private:
		
			CFMRadioManualTuningEditorIndicator();

			void ConstructL(CCoeControl* aControl);
            
            // from base class CAknIndicatorContainer
			/**
			* From CAknIndicatorContainer
			* 
			* @see CAknIndicatorContainer::Reserved_1()
			*/
			void Reserved_1();

		private:
			/** 
			 * Flags 
			 */
			TBitFlags16 iBitFlags;

		};
	
		public:
	    /**
	     * Manage indexes into LAF tables
	     */
	    class TIndex 
	    {   
	    public:     
	        TIndex(TInt aNumberOfPromptLines);  
	    
	    public: 
	        TInt PromptLine() const;    
	        
	    public: 
	        TInt DQPWindowTextsLine2(TInt aLineNum) const;  
	        TInt PNWindow() const;  
	        TInt PQDWindow(TInt aLineNum) const;    
	        TInt PQCWindow() const;
	        
	    private:    

	
	    private:    
	        TInt  iNumberOfPromptLines; 
	    };  
    
protected:  // Methods

    // from base class
    /**
     * From CAknQueryControl
     * 
     * @see CAknQueryControl::ConstructQueryL( TResourceReader& aRes )
     */
    void ConstructQueryL( TResourceReader& aRes );

	/**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutEditor( const TLayoutMethod& aLayoutM )
     */
	void LayoutEditor( const TLayoutMethod& aLayoutM );

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutPrompt( const TLayoutMethod& aLayoutM )
     */
	void LayoutPrompt( const TLayoutMethod& aLayoutM );

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutImageOrAnim( 
     *    const TLayoutMethod& aLayoutM )
     */
    void LayoutImageOrAnim( const TLayoutMethod& aLayoutM );

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutEditorFrame( 
     *    const TLayoutMethod& aLayoutM )
     */
    void LayoutEditorFrame( const TLayoutMethod& aLayoutM );

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutEditorIndicator( 
     *    const TLayoutMethod& aLayoutM )
     */
    void LayoutEditorIndicator( const TLayoutMethod& aLayoutM );

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::LayoutRect()
     */
	TRect LayoutRect();

    /**
     * From CAknQueryControl
     *
     * @see CAknQueryControl::SetLineWidthsL();
     */
	void SetLineWidthsL();

	/**
     * From CCoeControl
     *
     * @see CCoeControl::Draw( const TRect& aRect ) const
     */
    void Draw( const TRect& aRect ) const;

    /**
     * Draw editor frame and shadows
     */
     void DrawFrequencyEditorFrame(CWindowGc& aGc,TRect& aRect) const;
     
	/**
     * From CCoeControl
     *
     * @see CCoeControl::SizeChanged()
     */
    void SizeChanged();

	/**
    * From MObjectProvider
    *
    * @see MObjectProvider::MopSupplyObject( TTypeUid aId )
    */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    
    /**
    * From CCoeControl.
    * @see CCoeControl::HandlePointerEventL()
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
private:    // Methods
    
    /**
     * Starts timer to accelerate long press repeat interval
     */
    void StartKeypress();
    
    /**
     * Instructs frequency editor component to update the appropriate field
     * if it is possible (user is not typing numbers). Update is either
     * incrementing or decrementing depending on state of iChangeDirectionIncrease.
     * If it is ETrue, the desired action is to increment the value.
     */
    void DoIncrementOrDecrement();

    /**
     * Callback for iLongPressAccelerationTimer
     * Speeds up repeated key event interval.
     * See CPeriodic::Start()
     * 
     * @param aAny pointer to self
     * 
     * @return ignored
     */
    static TInt LongPressAccelerateCallBack( TAny* aAny );
    
    /**
     * Callback for iLongPressTimer
     * Called repeatedly to act as long keypress repeat event.
     * See CPeriodic::Start()
     * 
     * @return ignored
     */
    static TInt LongPressTimerCallBack( TAny* aAny );
    
private:    // Data

	/** 
	 * Prompt 
	 */
    CEikLabel* iMyPrompt;
    
	/**  
	 * Editor indicator 
	 */
    CFMRadioManualTuningEditorIndicator* iMyEditorIndicator;
	
	/** 
	 * Editor 
	 */
    CFMRadioFrequencyEditor*	iFreqEditor;
    
	/** 
	 * A control context that provides a layout background generated from a frame item 
	 */
    CAknsFrameBackgroundControlContext* iEditorContext;
     /**
     * Accelerates manual tuning speed after two seconds
     */
    CPeriodic* iLongPressAccelerationTimer;
    
    CPeriodic* iLongPressTimer;
    
    /**
     * State of current keypress. Value is valid only between EEventKeyDown
     * and EEventKeyUp.
     * ETrue == increment, EFalse == decrement
     */
    TBool iChangeDirectionIncrease;
    
    TBool iKeyPressReported;
    
    };

#endif      // FMRADIOFREQUENCYQUERYCONTROL_H
