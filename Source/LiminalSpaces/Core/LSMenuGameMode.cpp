#include "Core/LSMenuGameMode.h"
#include "UI/LSMainMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "LiminalSpaces.h"

ALSMenuGameMode::ALSMenuGameMode()
{
	// No pawn needed in menu
	DefaultPawnClass = nullptr;
}

void ALSMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Show mouse cursor and set up menu input mode
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());

		if (MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget<ULSMainMenuWidget>(PC, MainMenuWidgetClass);
			if (MainMenuWidget)
			{
				MainMenuWidget->AddToViewport(0);
				UE_LOG(LogLiminalSpaces, Log, TEXT("Main menu displayed"));
			}
		}
	}
}
