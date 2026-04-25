#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Level/LSInteractableBase.h"
#include "LSWeaponPickup.generated.h"

class ALSWeaponBase;
class UStaticMeshComponent;
class USphereComponent;
class UPointLightComponent;

/**
 * Pickup that grants the player a weapon when interacted with.
 */
UCLASS()
class LIMINALSPACES_API ALSWeaponPickup : public AActor, public ILSInteractable
{
	GENERATED_BODY()

public:
	ALSWeaponPickup();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Pickup")
	void SetWeaponClass(TSubclassOf<ALSWeaponBase> InClass) { WeaponClass = InClass; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Pickup")
	void SetPickupLineID(FName InID) { PickupLineID = InID; }

	virtual bool CanInteract_Implementation(ACharacter* InteractingCharacter) const override;
	virtual void Interact_Implementation(ACharacter* InteractingCharacter) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> Glow;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> InteractionVolume;

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Pickup")
	TSubclassOf<ALSWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Pickup")
	FText PickupPromptText = FText::FromString(TEXT("[E] Pick up"));

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Pickup")
	FName PickupLineID;

private:
	bool bConsumed = false;
	float TimeAccumulator = 0.0f;
	FVector InitialMeshLocation;
};
