#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/LSTypes.h"
#include "LSPlayerState.generated.h"

UCLASS()
class LIMINALSPACES_API ALSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALSPlayerState();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	const FLSPlayerStats& GetPlayerStats() const { return PlayerStats; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Player")
	void IncrementMapsCompleted();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Player")
	void IncrementEnemiesDefeated();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Player")
	void UpdateBestTime(float CompletionTime);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Player")
	void SetAuthUserID(const FString& InUserID);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	FString GetAuthUserID() const { return AuthUserID; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Stats")
	FLSPlayerStats PlayerStats;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Auth")
	FString AuthUserID;
};
