#include "Core/LSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ALSPlayerController::ALSPlayerController()
{
}

void ALSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create pause input action if not assigned
	if (!PauseAction)
	{
		PauseAction = NewObject<UInputAction>(this, TEXT("IA_Pause"));
		PauseAction->ValueType = EInputActionValueType::Boolean;
	}

	// Create mapping context with Escape key for pause
	if (!GameplayMappingContext)
	{
		GameplayMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Controller"));
		GameplayMappingContext->MapKey(PauseAction, EKeys::Escape);
	}

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
