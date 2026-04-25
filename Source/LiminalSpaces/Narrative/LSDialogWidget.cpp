#include "Narrative/LSDialogWidget.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Font.h"
#include "LiminalSpaces.h"

ULSDialogWidget::ULSDialogWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULSDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Build the UMG tree procedurally so we don't depend on a Blueprint asset
	if (!DialogText)
	{
		BuildWidgetTree();
	}

	// Bind to the narrative subsystem
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			Narr->OnDialogLineRequested.AddDynamic(this, &ULSDialogWidget::HandleDialogLine);
			Narr->OnNoteRead.AddDynamic(this, &ULSDialogWidget::HandleNoteRead);
		}
	}

	if (DialogBorder)
	{
		DialogBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	if (NoteBackdrop)
	{
		NoteBackdrop->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULSDialogWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			Narr->OnDialogLineRequested.RemoveDynamic(this, &ULSDialogWidget::HandleDialogLine);
			Narr->OnNoteRead.RemoveDynamic(this, &ULSDialogWidget::HandleNoteRead);
		}
	}

	Super::NativeDestruct();
}

void ULSDialogWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	// Root canvas
	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	// === DIALOG BORDER (bottom-center) ===
	DialogBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialogBorder"));
	DialogBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f));
	DialogBorder->SetPadding(FMargin(28.0f, 16.0f));

	DialogText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DialogText"));
	DialogText->SetText(FText::GetEmpty());
	DialogText->SetJustification(ETextJustify::Center);
	DialogText->SetAutoWrapText(true);
	{
		FSlateFontInfo Font = DialogText->GetFont();
		Font.Size = 22;
		DialogText->SetFont(Font);
	}
	DialogText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.92f, 0.85f)));

	DialogBorder->AddChild(DialogText);
	RootCanvas->AddChild(DialogBorder);

	if (UCanvasPanelSlot* DSlot = Cast<UCanvasPanelSlot>(DialogBorder->Slot))
	{
		DSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
		DSlot->SetAlignment(FVector2D(0.5f, 1.0f));
		DSlot->SetSize(FVector2D(900.0f, 110.0f));
		DSlot->SetPosition(FVector2D(0.0f, -90.0f));
		DSlot->SetAutoSize(false);
	}

	// === NOTE BACKDROP (full-screen overlay) ===
	NoteBackdrop = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("NoteBackdrop"));
	NoteBackdrop->SetBrushColor(FLinearColor(0.05f, 0.04f, 0.03f, 0.92f));
	NoteBackdrop->SetPadding(FMargin(80.0f));

	UVerticalBox* NoteBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("NoteBox"));

	NoteTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NoteTitle"));
	NoteTitle->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = NoteTitle->GetFont();
		Font.Size = 32;
		Font.TypefaceFontName = FName("Italic");
		NoteTitle->SetFont(Font);
	}
	NoteTitle->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.85f, 0.6f)));

	NoteBody = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NoteBody"));
	NoteBody->SetAutoWrapText(true);
	NoteBody->SetJustification(ETextJustify::Left);
	{
		FSlateFontInfo Font = NoteBody->GetFont();
		Font.Size = 20;
		NoteBody->SetFont(Font);
	}
	NoteBody->SetColorAndOpacity(FSlateColor(FLinearColor(0.92f, 0.88f, 0.78f)));

	NoteHint = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NoteHint"));
	NoteHint->SetText(FText::FromString(TEXT("[E] to continue")));
	NoteHint->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = NoteHint->GetFont();
		Font.Size = 16;
		NoteHint->SetFont(Font);
	}
	NoteHint->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.55f, 0.45f)));

	NoteBox->AddChild(NoteTitle);
	if (UVerticalBoxSlot* TitleSlot = Cast<UVerticalBoxSlot>(NoteTitle->Slot))
	{
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 24.0f));
		TitleSlot->SetHorizontalAlignment(HAlign_Center);
	}
	NoteBox->AddChild(NoteBody);
	if (UVerticalBoxSlot* BodySlot = Cast<UVerticalBoxSlot>(NoteBody->Slot))
	{
		BodySlot->SetPadding(FMargin(40.0f, 0.0f, 40.0f, 32.0f));
	}
	NoteBox->AddChild(NoteHint);

	NoteBackdrop->AddChild(NoteBox);
	RootCanvas->AddChild(NoteBackdrop);

	if (UCanvasPanelSlot* NSlot = Cast<UCanvasPanelSlot>(NoteBackdrop->Slot))
	{
		NSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		NSlot->SetOffsets(FMargin(0.0f));
	}

	// === CLUE COUNTER (top-right) ===
	ClueCounterText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ClueCounter"));
	ClueCounterText->SetText(FText::GetEmpty());
	ClueCounterText->SetJustification(ETextJustify::Right);
	{
		FSlateFontInfo Font = ClueCounterText->GetFont();
		Font.Size = 18;
		ClueCounterText->SetFont(Font);
	}
	ClueCounterText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.80f, 0.55f, 0.85f)));

	RootCanvas->AddChild(ClueCounterText);
	if (UCanvasPanelSlot* CSlot = Cast<UCanvasPanelSlot>(ClueCounterText->Slot))
	{
		CSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
		CSlot->SetAlignment(FVector2D(1.0f, 0.0f));
		CSlot->SetPosition(FVector2D(-30.0f, 30.0f));
		CSlot->SetAutoSize(true);
	}
}

void ULSDialogWidget::HandleDialogLine(const FLSDialogLine& Line)
{
	if (!DialogBorder || !DialogText)
	{
		return;
	}

	FullText = Line.Text.ToString();
	DisplayedText.Empty();
	TypeAccumulator = 0.0f;
	HoldTimeRemaining = Line.HoldDuration;
	bIsTyping = true;
	bIsHolding = false;
	bIsFadingOut = false;
	FadeAlpha = 1.0f;
	CurrentMood = Line.Mood;

	// Mood-based tinting
	FLinearColor TextColor;
	FLinearColor BgColor;
	switch (Line.Mood)
	{
		case ELSDialogMood::Tense:
			TextColor = FLinearColor(1.0f, 0.85f, 0.78f);
			BgColor = FLinearColor(0.18f, 0.05f, 0.05f, 0.78f);
			break;
		case ELSDialogMood::Danger:
			TextColor = FLinearColor(1.0f, 0.55f, 0.50f);
			BgColor = FLinearColor(0.30f, 0.05f, 0.05f, 0.85f);
			break;
		case ELSDialogMood::Awe:
			TextColor = FLinearColor(0.95f, 0.95f, 0.85f);
			BgColor = FLinearColor(0.05f, 0.06f, 0.10f, 0.65f);
			break;
		case ELSDialogMood::Discovery:
			TextColor = FLinearColor(1.0f, 0.92f, 0.65f);
			BgColor = FLinearColor(0.10f, 0.08f, 0.05f, 0.75f);
			break;
		case ELSDialogMood::Resolution:
			TextColor = FLinearColor(0.95f, 0.95f, 0.95f);
			BgColor = FLinearColor(0.10f, 0.10f, 0.14f, 0.78f);
			break;
		case ELSDialogMood::Confused:
			TextColor = FLinearColor(0.85f, 0.83f, 0.80f);
			BgColor = FLinearColor(0.06f, 0.06f, 0.07f, 0.70f);
			break;
		case ELSDialogMood::Curious:
			TextColor = FLinearColor(0.92f, 0.93f, 0.90f);
			BgColor = FLinearColor(0.05f, 0.07f, 0.06f, 0.65f);
			break;
		case ELSDialogMood::Calm:
		default:
			TextColor = FLinearColor(0.95f, 0.92f, 0.85f);
			BgColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.60f);
			break;
	}

	DialogText->SetColorAndOpacity(FSlateColor(TextColor));
	DialogBorder->SetBrushColor(BgColor);
	DialogText->SetText(FText::GetEmpty());
	DialogBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
	DialogBorder->SetRenderOpacity(1.0f);
}

void ULSDialogWidget::HandleNoteRead(FText Title, FText Body)
{
	if (!NoteBackdrop)
	{
		return;
	}

	if (NoteTitle)
	{
		NoteTitle->SetText(Title);
	}
	if (NoteBody)
	{
		NoteBody->SetText(Body);
	}
	NoteBackdrop->SetVisibility(ESlateVisibility::Visible);
	bIsNoteOpen = true;

	// Auto-dismiss after a generous read time (player can also press E to dismiss early)
	if (UWorld* World = GetWorld())
	{
		NoteAutoDismissAt = World->GetTimeSeconds() + 14.0f;
	}

	// Take focus so we receive key events
	SetKeyboardFocus();
}

void ULSDialogWidget::DismissNote()
{
	if (NoteBackdrop)
	{
		NoteBackdrop->SetVisibility(ESlateVisibility::Collapsed);
	}
	bIsNoteOpen = false;
}

FReply ULSDialogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsNoteOpen)
	{
		const FKey Key = InKeyEvent.GetKey();
		if (Key == EKeys::E || Key == EKeys::SpaceBar || Key == EKeys::Enter
			|| Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Bottom
			|| Key == EKeys::Gamepad_FaceButton_Left)
		{
			DismissNote();
			return FReply::Handled();
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULSDialogWidget::ShowClueProgress(int32 Collected, int32 Total)
{
	if (ClueCounterText)
	{
		const FString Display = FString::Printf(TEXT("Memories: %d / %d"), Collected, Total);
		ClueCounterText->SetText(FText::FromString(Display));
	}
}

void ULSDialogWidget::TickTypewriter(float DeltaTime)
{
	if (bIsTyping)
	{
		TypeAccumulator += DeltaTime * CharsPerSecond;
		const int32 TargetChars = FMath::Min(FullText.Len(), FMath::FloorToInt(TypeAccumulator));
		if (TargetChars > DisplayedText.Len())
		{
			DisplayedText = FullText.Left(TargetChars);
			if (DialogText)
			{
				DialogText->SetText(FText::FromString(DisplayedText));
			}
		}
		if (DisplayedText.Len() >= FullText.Len())
		{
			bIsTyping = false;
			bIsHolding = true;
		}
	}
	else if (bIsHolding)
	{
		HoldTimeRemaining -= DeltaTime;
		if (HoldTimeRemaining <= 0.0f)
		{
			bIsHolding = false;
			bIsFadingOut = true;
		}
	}
	else if (bIsFadingOut)
	{
		FadeAlpha = FMath::Max(0.0f, FadeAlpha - DeltaTime * 1.5f);
		if (DialogBorder)
		{
			DialogBorder->SetRenderOpacity(FadeAlpha);
		}
		if (FadeAlpha <= 0.0f)
		{
			bIsFadingOut = false;
			if (DialogBorder)
			{
				DialogBorder->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void ULSDialogWidget::NativeTick(const FGeometry& MovedGeometry, float InDeltaTime)
{
	Super::NativeTick(MovedGeometry, InDeltaTime);
	TickTypewriter(InDeltaTime);

	if (bIsNoteOpen)
	{
		if (UWorld* World = GetWorld())
		{
			if (World->GetTimeSeconds() >= NoteAutoDismissAt)
			{
				DismissNote();
			}
		}
	}
}
