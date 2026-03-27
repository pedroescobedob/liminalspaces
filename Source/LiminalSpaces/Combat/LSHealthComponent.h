#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup = (LiminalSpaces), meta = (BlueprintSpawnableComponent))
class LIMINALSPACES_API ULSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULSHealthComponent();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Health")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Health")
	bool IsDead() const { return bIsDead; }

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

private:
	float CurrentHealth;
	bool bIsDead = false;
};
