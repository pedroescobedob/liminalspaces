#include "Core/LiminalSpacesGameInstance.h"
#include "Core/LSSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ULiminalSpacesGameInstance::ULiminalSpacesGameInstance()
{
}

void ULiminalSpacesGameInstance::Init()
{
	Super::Init();

	LoadSettings();

	UE_LOG(LogLiminalSpaces, Log, TEXT("Liminal Spaces Game Instance initialized"));
}

void ULiminalSpacesGameInstance::SetAuthSession(const FLSAuthSession& InSession)
{
	AuthSession = InSession;
}

void ULiminalSpacesGameInstance::ClearAuthSession()
{
	AuthSession = FLSAuthSession();
}

void ULiminalSpacesGameInstance::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void ULiminalSpacesGameInstance::SetMouseSensitivity(float Sensitivity)
{
	MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
}

void ULiminalSpacesGameInstance::SaveSettings()
{
	if (ULSSaveGame* SaveGameInstance = Cast<ULSSaveGame>(UGameplayStatics::CreateSaveGameObject(ULSSaveGame::StaticClass())))
	{
		SaveGameInstance->MasterVolume = MasterVolume;
		SaveGameInstance->MouseSensitivity = MouseSensitivity;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Settings"), 0);
	}
}

void ULiminalSpacesGameInstance::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("Settings"), 0))
	{
		if (ULSSaveGame* SaveGameInstance = Cast<ULSSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Settings"), 0)))
		{
			MasterVolume = SaveGameInstance->MasterVolume;
			MouseSensitivity = SaveGameInstance->MouseSensitivity;
		}
	}
}
