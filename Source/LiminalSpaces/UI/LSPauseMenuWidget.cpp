#include "UI/LSPauseMenuWidget.h"
#include "Components/Button.h"
#include "Core/LSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ULSPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &ULSPauseMenuWidget::OnResumeClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &ULSPauseMenuWidget::OnSettingsClicked);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &ULSPauseMenuWidget::OnMainMenuClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &ULSPauseMenuWidget::OnQuitClicked);
	}
}

void ULSPauseMenuWidget::OnResumeClicked()
{
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetOwningPlayer()))
	{
		PC->HidePauseMenu();
	}
	RemoveFromParent();
}

void ULSPauseMenuWidget::OnSettingsClicked()
{
	// Override in Blueprint
}

void ULSPauseMenuWidget::OnMainMenuClicked()
{
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetOwningPlayer()))
	{
		PC->HidePauseMenu();
		PC->ReturnToMainMenu();
	}
}

void ULSPauseMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
