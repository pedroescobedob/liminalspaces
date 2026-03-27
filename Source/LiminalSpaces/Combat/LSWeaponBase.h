#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/LSTypes.h"
#include "LSWeaponBase.generated.h"

class USkeletalMeshComponent;
class UNiagaraSystem;
class USoundBase;
class UAnimMontage;

UCLASS(Abstract)
class LIMINALSPACES_API ALSWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ALSWeaponBase();

	virtual void StartFire();
	virtual void StopFire();
	virtual void Reload();

	void SetOwningCharacter(ACharacter* InCharacter);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Weapon")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Weapon")
	ELSWeaponType GetWeaponType() const { return WeaponType; }

protected:
	virtual void BeginPlay() override;
	virtual void Fire();
	void PlayFireEffects();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	ELSWeaponType WeaponType = ELSWeaponType::Pistol;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float Damage = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float FireRate = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	float Range = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	bool bIsAutomatic = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon|FX")
	TObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon|FX")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon|FX")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Weapon")
	FName MuzzleSocketName = FName("Muzzle");

	UPROPERTY()
	TObjectPtr<ACharacter> OwningCharacter;

	int32 CurrentAmmo;
	float LastFireTime = 0.0f;
	bool bWantsToFire = false;

	FTimerHandle FireTimerHandle;

	void HandleAutoFire();
};
