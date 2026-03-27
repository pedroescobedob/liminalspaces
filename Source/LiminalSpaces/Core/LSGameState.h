#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/LSTypes.h"
#include "LSGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameProgressChanged, ELSGameState, NewState);

UCLASS()
class LIMINALSPACES_API ALSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALSGameState();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|GameState")
	void SetGameProgress(ELSGameState NewState);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|GameState")
	ELSGameState GetGameProgress() const { return CurrentGameState; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|GameState")
	int32 GetEnemiesRemaining() const { return EnemiesRemaining; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|GameState")
	void SetEnemiesRemaining(int32 Count);

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnGameProgressChanged OnGameProgressChanged;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|GameState")
	ELSGameState CurrentGameState = ELSGameState::WaitingToStart;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|GameState")
	int32 EnemiesRemaining = 0;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|GameState")
	float MapStartTime = 0.0f;
};
