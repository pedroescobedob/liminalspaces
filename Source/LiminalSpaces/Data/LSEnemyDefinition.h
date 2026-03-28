#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/LSTypes.h"
#include "LSEnemyDefinition.generated.h"

class ALSEnemyBase;

UCLASS(BlueprintType)
class LIMINALSPACES_API ULSEnemyDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	TSubclassOf<ALSEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	TSubclassOf<UAnimInstance> AnimBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float BaseHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float DetectionRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	TObjectPtr<UTexture2D> Portrait;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("LSEnemy"), GetFName());
	}
};
