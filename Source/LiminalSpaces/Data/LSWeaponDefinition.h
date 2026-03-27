#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/LSTypes.h"
#include "LSWeaponDefinition.generated.h"

class ALSWeaponBase;
class UNiagaraSystem;

UCLASS(BlueprintType)
class LIMINALSPACES_API ULSWeaponDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	ELSWeaponType WeaponType = ELSWeaponType::Pistol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	TSubclassOf<ALSWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float FireRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float Range = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	int32 MaxAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	bool bIsAutomatic = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon|FX")
	TObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon|FX")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	TObjectPtr<UTexture2D> Icon;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("LSWeapon"), GetFName());
	}
};
