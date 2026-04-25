#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Level/LSInteractableBase.h"
#include "LSClueNote.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class USphereComponent;

/**
 * Pickup actor for a story clue (note, photo, journal page).
 * The narrative subsystem owns the actual content; this actor just references a ClueID.
 */
UCLASS()
class LIMINALSPACES_API ALSClueNote : public AActor, public ILSInteractable
{
	GENERATED_BODY()

public:
	ALSClueNote();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void SetClueID(FName InID) { ClueID = InID; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	FName GetClueID() const { return ClueID; }

	// ILSInteractable
	virtual bool CanInteract_Implementation(ACharacter* InteractingCharacter) const override;
	virtual void Interact_Implementation(ACharacter* InteractingCharacter) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NoteMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> Glow;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> InteractionVolume;

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Narrative")
	FName ClueID;

private:
	bool bConsumed = false;
	float TimeAccumulator = 0.0f;
	FVector InitialMeshLocation;
};
