#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/LSTypes.h"
#include "LSEnemyBase.generated.h"

class ULSHealthComponent;
class UBehaviorTree;
class UAnimMontage;
class UNiagaraSystem;

UCLASS(Abstract)
class LIMINALSPACES_API ALSEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ALSEnemyBase();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Enemy")
	ULSHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Enemy")
	ELSEnemyState GetEnemyState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Enemy")
	void SetEnemyState(ELSEnemyState NewState);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Enemy")
	void PerformAttack();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Health")
	TObjectPtr<ULSHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	FString EnemyDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float AttackDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float AttackRange = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float DetectionRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy|FX")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy|FX")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy|FX")
	TObjectPtr<USoundBase> AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy|FX")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy|FX")
	TObjectPtr<UNiagaraSystem> DeathFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	float DeathDestroyDelay = 5.0f;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Enemy")
	ELSEnemyState CurrentState = ELSEnemyState::Idle;

	float LastAttackTime = 0.0f;
};
