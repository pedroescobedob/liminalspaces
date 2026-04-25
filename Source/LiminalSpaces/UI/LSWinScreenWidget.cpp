#include "UI/LSWinScreenWidget.h"
#include "Core/LSPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

ULSWinScreenWidget::ULSWinScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULSWinScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!TitleText)
	{
		BuildWidgetTree();
	}
	SetKeyboardFocus();
}

void ULSWinScreenWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("WinRoot"));
	WidgetTree->RootWidget = RootCanvas;

	Backdrop = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Backdrop"));
	Backdrop->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.04f, 0.97f));
	Backdrop->SetPadding(FMargin(60.0f));
	RootCanvas->AddChild(Backdrop);
	if (UCanvasPanelSlot* BSlot = Cast<UCanvasPanelSlot>(Backdrop->Slot))
	{
		BSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		BSlot->SetOffsets(FMargin(0.0f));
	}

	UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box"));
	Backdrop->AddChild(Box);

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	TitleText->SetText(FText::FromString(TEXT("you wake")));
	TitleText->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = TitleText->GetFont();
		Font.Size = 64;
		TitleText->SetFont(Font);
	}
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.92f, 0.78f)));

	SubtitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Subtitle"));
	SubtitleText->SetText(FText::FromString(TEXT("the hallway is gone.\nthere is a window. there is light.\nshe is holding your hand.")));
	SubtitleText->SetAutoWrapText(true);
	SubtitleText->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = SubtitleText->GetFont();
		Font.Size = 22;
		SubtitleText->SetFont(Font);
	}
	SubtitleText->SetColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.74f, 0.65f)));

	StatsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Stats"));
	StatsText->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = StatsText->GetFont();
		Font.Size = 18;
		StatsText->SetFont(Font);
	}
	StatsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.55f)));

	CreditsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Credits"));
	CreditsText->SetText(FText::FromString(TEXT("LIMINAL SPACES\n\nA story about waking up.\n\nThanks for playing.")));
	CreditsText->SetAutoWrapText(true);
	CreditsText->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = CreditsText->GetFont();
		Font.Size = 18;
		CreditsText->SetFont(Font);
	}
	CreditsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.55f, 0.50f)));

	HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Hint"));
	HintText->SetText(FText::FromString(TEXT("[Enter] return to main menu")));
	HintText->SetJustification(ETextJustify::Center);
	{
		FSlateFontInfo Font = HintText->GetFont();
		Font.Size = 14;
		HintText->SetFont(Font);
	}
	HintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f)));

	auto AddBoxChild = [Box](UWidget* Child, float TopPad, float BottomPad)
	{
		Box->AddChild(Child);
		if (UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(Child->Slot))
		{
			Slot->SetPadding(FMargin(0.0f, TopPad, 0.0f, BottomPad));
			Slot->SetHorizontalAlignment(HAlign_Center);
		}
	};
	AddBoxChild(TitleText, 80.0f, 24.0f);
	AddBoxChild(SubtitleText, 0.0f, 64.0f);
	AddBoxChild(StatsText, 0.0f, 48.0f);
	AddBoxChild(CreditsText, 0.0f, 64.0f);
	AddBoxChild(HintText, 0.0f, 0.0f);
}

void ULSWinScreenWidget::ShowResults(float CompletionTime, int32 CluesCollected, int32 CluesTotal)
{
	if (!StatsText)
	{
		BuildWidgetTree();
	}
	const int32 Minutes = FMath::FloorToInt(CompletionTime / 60.0f);
	const int32 Seconds = FMath::FloorToInt(FMath::Fmod(CompletionTime, 60.0f));
	const FString S = FString::Printf(TEXT("Time: %02d:%02d\nMemories recovered: %d / %d"),
		Minutes, Seconds, CluesCollected, CluesTotal);
	if (StatsText)
	{
		StatsText->SetText(FText::FromString(S));
	}
}

void ULSWinScreenWidget::NativeTick(const FGeometry& MovedGeometry, float InDeltaTime)
{
	Super::NativeTick(MovedGeometry, InDeltaTime);
	FadeAlpha = FMath::Min(1.0f, FadeAlpha + InDeltaTime * 0.6f);
	SetRenderOpacity(FadeAlpha);
}

FReply ULSWinScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar
		|| Key == EKeys::Gamepad_FaceButton_Bottom || Key == EKeys::Gamepad_Special_Right)
	{
		ReturnToMenu();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULSWinScreenWidget::ReturnToMenu()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ALSPlayerController* LSPC = Cast<ALSPlayerController>(PC))
			{
				LSPC->ReturnToMainMenu();
				return;
			}
			UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/LiminalSpaces/Maps/MainMenu/Lvl_MainMenu")));
		}
	}
}
