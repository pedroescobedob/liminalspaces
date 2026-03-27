#include "Core/LSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ALSPlayerController::ALSPlayerController()
{
}

void ALSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (GameplayMappingContext)
		{
			Subsystem->AddMappingContext(GameplayMappingContext, 0);
		}
	}
}

void ALSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &ALSPlayerController::HandlePauseInput);
		}
	}
}

void ALSPlayerController::HandlePauseInput()
{
	TogglePauseMenu();
}

void ALSPlayerController::ShowPauseMenu()
{
	bIsPaused = true;
	SetPause(true);
	SetShowMouseCursor(true);
	SetInputMode(FInputModeGameAndUI());
}

void ALSPlayerController::HidePauseMenu()
{
	bIsPaused = false;
	SetPause(false);
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void ALSPlayerController::TogglePauseMenu()
{
	if (bIsPaused)
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void ALSPlayerController::TravelToMap(const FString& MapPath)
{
	UGameplayStatics::OpenLevel(this, FName(*MapPath));
}

void ALSPlayerController::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/LiminalSpaces/Maps/MainMenu/Lvl_MainMenu")));
}
