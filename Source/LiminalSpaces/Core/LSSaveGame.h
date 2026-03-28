#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/LSTypes.h"
#include "LSSaveGame.generated.h"

UCLASS()
class LIMINALSPACES_API ULSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	ULSSaveGame();

	UPROPERTY(SaveGame)
	float MasterVolume = 1.0f;

	UPROPERTY(SaveGame)
	float MouseSensitivity = 1.0f;

	UPROPERTY(SaveGame)
	FLSPlayerStats PlayerStats;

	UPROPERTY(SaveGame)
	TArray<FString> CompletedMapIDs;

	UPROPERTY(SaveGame)
	TMap<FString, float> MapBestTimes;
};
