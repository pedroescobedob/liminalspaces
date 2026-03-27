#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LSGameMode.generated.h"

class ALSEscapePoint;
class ALSBedCellActor;
class ULSMapDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEscaped, APlayerController*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemiesDefeated);

UCLASS()
class LIMINALSPACES_API ALSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALSGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Game")
	void OnEscapeReached(APlayerController* Player);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Game")
	void OnEnemyDefeated();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Game")
	void RestartMap(APlayerController* Player);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Game")
	float GetElapsedTime() const;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnPlayerEscaped OnPlayerEscaped;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnAllEnemiesDefeated OnAllEnemiesDefeated;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	TObjectPtr<ULSMapDefinition> CurrentMapDefinition;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Game")
	int32 TotalEnemies = 0;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Game")
	int32 EnemiesRemaining = 0;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Game")
	float MapStartTime = 0.0f;

	void SpawnEnemies();
};
