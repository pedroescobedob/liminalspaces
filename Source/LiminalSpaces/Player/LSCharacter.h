#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class ULSHealthComponent;
class ULSCombatComponent;
class ULSInteractionComponent;
class UPostProcessComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UCameraShakeBase;

UCLASS()
class LIMINALSPACES_API ALSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALSCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	ULSHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	ULSCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	ULSInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	float GetStaminaPercent() const { return MaxStamina > 0.0f ? (CurrentStamina / MaxStamina) : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	float GetStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Player")
	float GetMaxStamina() const { return MaxStamina; }

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Health")
	TObjectPtr<ULSHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Combat")
	TObjectPtr<ULSCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Interaction")
	TObjectPtr<ULSInteractionComponent> InteractionComponent;

	// First person mesh (arms)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// VHS / Bodycam post process — applied per-character so it follows the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UPostProcessComponent> CameraPostProcess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UMaterialInterface> VHSBaseMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UMaterialInterface> NoiseBaseMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UMaterialInterface> AberrationBaseMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> NoiseMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> AberrationMID;

	// Camera shake classes from the Bodycam VHS pack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|CameraShake")
	TSubclassOf<UCameraShakeBase> IdleCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|CameraShake")
	TSubclassOf<UCameraShakeBase> WalkCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|CameraShake")
	TSubclassOf<UCameraShakeBase> RunCameraShakeClass;

	// Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> InteractAction;

	// Movement settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Movement")
	float SprintSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Movement")
	float CrouchSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Camera")
	float LookSensitivity = 1.0f;

	// === Stamina ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Stamina")
	float StaminaDrainPerSecond = 22.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Stamina")
	float StaminaRegenPerSecond = 14.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Stamina")
	float StaminaRegenDelay = 1.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Combat")
	TSubclassOf<class ALSWeaponBase> StarterUnarmedWeaponClass;

private:
	void CreateInputActions();
	void UpdateCameraShakeForMovement();
	void UpdatePostProcessIntensity(float DeltaTime);
	void InitializePostProcess();
	TSubclassOf<UCameraShakeBase> CurrentShakeClass;
	float CurrentDangerBlend = 0.0f;
	void HandleMove(const struct FInputActionValue& Value);
	void HandleLook(const struct FInputActionValue& Value);
	void HandleJump();
	void HandleStopJump();
	void HandleSprintStart();
	void HandleSprintStop();
	void HandleCrouchToggle();
	void HandleAttack();
	void HandleStopAttack();
	void HandleInteract();
	UFUNCTION()
	void OnDeath();

	bool bIsSprinting = false;
	float CurrentStamina = 100.0f;
	float TimeOfLastStaminaUse = -100.0f;
	float NextAmbientLineTime = 0.0f;
};
