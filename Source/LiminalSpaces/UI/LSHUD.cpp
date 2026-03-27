#include "UI/LSHUD.h"
#include "UI/LSHUDWidget.h"
#include "UI/LSPauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "LiminalSpaces.h"

ALSHUD::ALSHUD()
{
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
