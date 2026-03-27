#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LSTypes.generated.h"

UENUM(BlueprintType)
enum class ELSGameState : uint8
{
	WaitingToStart,
	InProgress,
	Escaped,
	Failed
};

UENUM(BlueprintType)
enum class ELSEnemyState : uint8
{
	Idle,
	Patrolling,
	Alert,
	Chasing,
	Attacking,
	Dead
};

UENUM(BlueprintType)
enum class ELSMapDifficulty : uint8
{
	Easy,
	Medium,
	Hard,
	Nightmare
};

UENUM(BlueprintType)
enum class ELSWeaponType : uint8
{
	Pistol,
	Rifle,
	Shotgun,
	Melee
};

UENUM(BlueprintType)
enum class ELSDamageType : uint8
{
	Projectile,
	Melee,
	Environmental,
	Explosion
};

USTRUCT(BlueprintType)
struct FLSEnemySpawnEntry : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ALSEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnProbability = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RespawnDelay = 30.0f;
};

USTRUCT(BlueprintType)
struct FLSAuthSession
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString AccessToken;

	UPROPERTY(BlueprintReadOnly)
	FString RefreshToken;

	UPROPERTY(BlueprintReadOnly)
	FString UserID;

	UPROPERTY(BlueprintReadOnly)
	FString Email;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid = false;
};

USTRUCT(BlueprintType)
struct FLSPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MapsCompleted = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnemiesDefeated = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPlayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BestCompletionTime = 0.0f;
};
