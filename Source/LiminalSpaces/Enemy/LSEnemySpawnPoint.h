#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/LSTypes.h"
#include "LSEnemySpawnPoint.generated.h"

class ALSEnemyBase;

UCLASS()
class LIMINALSPACES_API ALSEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ALSEnemySpawnPoint();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Spawn")
	bool TrySpawnEnemy();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Spawn")
	void RespawnEnemy();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Spawn")
	bool HasSpawnedEnemy() const { return SpawnedEnemy != nullptr; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Spawn")
	TSubclassOf<ALSEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Spawn", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnProbability = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Spawn")
	bool bRespawnOnDeath = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Spawn", meta = (EditCondition = "bRespawnOnDeath"))
	float RespawnDelay = 30.0f;

private:
	UPROPERTY()
	TObjectPtr<ALSEnemyBase> SpawnedEnemy;

	FTimerHandle RespawnTimerHandle;

	UFUNCTION()
	void OnSpawnedEnemyDeath();
};
