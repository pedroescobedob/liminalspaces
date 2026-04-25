#include "UI/LSHUD.h"
#include "UI/LSHUDWidget.h"
#include "UI/LSPauseMenuWidget.h"
#include "UI/LSWinScreenWidget.h"
#include "Narrative/LSDialogWidget.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "LiminalSpaces.h"

ALSHUD::ALSHUD()
{
	// Use our self-constructing dialog widget by default
	DialogWidgetClass = ULSDialogWidget::StaticClass();
	WinScreenWidgetClass = ULSWinScreenWidget::StaticClass();
}

void ALSHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<ULSHUDWidget>(PC, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(0);
		}
	}

	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<ULSPauseMenuWidget>(PC, PauseMenuWidgetClass);
	}

	// Always create the dialog widget; it self-builds its UMG tree if no Blueprint is set
	if (DialogWidgetClass)
	{
		DialogWidget = CreateWidget<ULSDialogWidget>(PC, DialogWidgetClass);
		if (DialogWidget)
		{
			DialogWidget->AddToViewport(50);
		}
	}

	// Bind clue collection so we can show progress on the HUD
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			Narr->OnClueCollected.AddDynamic(this, &ALSHUD::HandleClueCollected);

			// Initialize counter display
			if (DialogWidget)
			{
				DialogWidget->ShowClueProgress(Narr->GetCluesCollectedCount(), Narr->GetTotalClueCount());
			}
		}
	}
}

void ALSHUD::HandleClueCollected(FName ClueID)
{
	if (!DialogWidget)
	{
		return;
	}
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			DialogWidget->ShowClueProgress(Narr->GetCluesCollectedCount(), Narr->GetTotalClueCount());
		}
	}
}

void ALSHUD::ShowHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void ALSHUD::HideHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ALSHUD::ShowPauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->AddToViewport(10);
		HideHUD();
	}
}

void ALSHUD::HidePauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		ShowHUD();
	}
}

void ALSHUD::ShowEscapeScreen(float CompletionTime)
{
	if (HUDWidget)
	{
		HUDWidget->ShowEscapeMessage(CompletionTime);
	}
}

void ALSHUD::ShowWinScreen(float CompletionTime, int32 CluesCollected)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	if (!WinScreenWidget && WinScreenWidgetClass)
	{
		WinScreenWidget = CreateWidget<ULSWinScreenWidget>(PC, WinScreenWidgetClass);
	}

	if (WinScreenWidget)
	{
		HideHUD();
		WinScreenWidget->AddToViewport(100);

		int32 TotalClues = 0;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
			{
				TotalClues = Narr->GetTotalClueCount();
			}
		}
		WinScreenWidget->ShowResults(CompletionTime, CluesCollected, TotalClues);

		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}
