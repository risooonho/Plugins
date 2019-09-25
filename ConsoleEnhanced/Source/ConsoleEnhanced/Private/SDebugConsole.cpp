// Copyright Michael Galetzka, 2017

#include "SDebugConsole.h"
#include "ConsoleEnhanced.h"
#include "SOutputLog.h"
#include "EditorStyleSet.h"
#include "SlateOptMacros.h"

namespace DebugConsoleDefs
{
	// How many seconds to animate when console is summoned
	static const float IntroAnimationDuration = 0.25f;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDebugConsole::Construct( const FArguments& InArgs, const EDebugConsoleStyle::Type InStyle, FConsoleEnhancedModule* OutputLogModule, const FDebugConsoleDelegates* DebugConsoleDelegates )
{
	CurrentStyle = InStyle;

	TSharedPtr<SConsoleInputBox> ConsoleInputBox;

	check( OutputLogModule != NULL );
	ChildSlot
	[ 
		SNew( SVerticalBox )
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew( SVerticalBox )
				.Visibility( this, &SDebugConsole::MakeVisibleIfLogIsShown )
					
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding( 10.0f )
				[
					SNew(SBox)
					.HeightOverride( 200.0f )
					[
						SNew( SBorder )
							.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
							.ColorAndOpacity( this, &SDebugConsole::GetAnimatedColorAndOpacity )
							.BorderBackgroundColor( this, &SDebugConsole::GetAnimatedSlateColor )
							[
								SNew( SSpacer )
							]
					]
				]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding( 10.0f )
		[
			SNew(SBox)
			.HeightOverride( 26.0f )
			.HAlign( HAlign_Left )
			[
				SNew( SBorder )
				.Padding( FMargin(2) )
				.BorderImage( FEditorStyle::GetBrush( "DebugConsole.Background" ) )
				.ColorAndOpacity( this, &SDebugConsole::GetAnimatedColorAndOpacity )
				.BorderBackgroundColor( this, &SDebugConsole::GetAnimatedSlateColor )
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(3.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Console", "ConsoleLabel", "Console"))

					]
					+ SHorizontalBox::Slot()
					.Padding(5.0f, 2.0f)
					.VAlign(VAlign_Center)
					.MaxWidth(400.0f)
					[
						SAssignNew(ConsoleInputBox, SConsoleInputBox)
						.OnConsoleCommandExecuted(DebugConsoleDelegates->OnConsoleCommandExecuted)
					]
				]
			]
		]
	];

	EditableTextBox = ConsoleInputBox->GetEditableTextBox();

	// Kick off intro animation
	AnimCurveSequence = FCurveSequence();
	AnimCurve = AnimCurveSequence.AddCurve( 0.0f, DebugConsoleDefs::IntroAnimationDuration, ECurveEaseFunction::QuadOut );
	FlashCurve = AnimCurveSequence.AddCurve( DebugConsoleDefs::IntroAnimationDuration, .15f, ECurveEaseFunction::QuadInOut );

	AnimCurveSequence.Play(this->AsShared());
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SDebugConsole::SDebugConsole()
	: CurrentStyle( EDebugConsoleStyle::Compact )
{
}


void SDebugConsole::SetFocusToEditableText()
{
	FSlateApplication::Get().SetKeyboardFocus( EditableTextBox.ToSharedRef(), EFocusCause::SetDirectly );
}

EVisibility SDebugConsole::MakeVisibleIfLogIsShown() const
{
	return CurrentStyle == EDebugConsoleStyle::WithLog ? EVisibility::Visible : EVisibility::Collapsed;
}


FLinearColor SDebugConsole::GetAnimatedColorAndOpacity() const
{
	return FLinearColor( 1.0f, 1.0f, 1.0f, AnimCurve.GetLerp() );
}


FSlateColor SDebugConsole::GetAnimatedSlateColor() const
{
	return FSlateColor( GetAnimatedColorAndOpacity() );
}

FSlateColor SDebugConsole::GetFlashColor() const
{
	float FlashAlpha = 1.0f - FlashCurve.GetLerp();

	if (FlashAlpha == 1.0f)
	{
		FlashAlpha = 0.0f;
	}

	return FLinearColor(1,1,1,FlashAlpha);
}