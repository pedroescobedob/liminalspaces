#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSCombatComponent.generated.h"

class ALSWeaponBase;
class ULSWeaponDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, ALSWeaponBase*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS(ClassGroup = (LiminalSpaces), meta = (BlueprintSpawnableComponent))
class LIMINALSPACES_API ULSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULSCombatComponent();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Combat")
	void EquipWeapon(TSubclassOf<ALSWeaponBase> WeaponClass);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Combat")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Combat")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Combat")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Combat")
	void Reload();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Combat")
	ALSWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Combat")
	bool HasWeapon() const { return CurrentWeapon != nullptr; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Combat")
	bool IsFiring() const { return bIsFiring; }

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnWeaponChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Combat")
	TSubclassOf<ALSWeaponBase> DefaultWeaponClass;

private:
	UPROPERTY()
	TObjectPtr<ALSWeaponBase> CurrentWeapon;

	bool bIsFiring = false;
};
