#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Level/LSInteractableBase.h"
#include "LSDoorActor.generated.h"

class UStaticMeshComponent;
class USoundBase;
class UTimelineComponent;

UENUM(BlueprintType)
enum class ELSDoorState : uint8
{
	Closed,
	Opening,
	Open,
	Closing
};

UCLASS()
class LIMINALSPACES_API ALSDoorActor : public AActor, public ILSInteractable
{
	GENERATED_BODY()

public:
	ALSDoorActor();

	// ILSInteractable interface
	virtual bool CanInteract_Implementation(ACharacter* InteractingCharacter) const override;
	virtual void Interact_Implementation(ACharacter* InteractingCharacter) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Door")
	void ToggleDoor();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Door")
	ELSDoorState GetDoorState() const { return DoorState; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	TObjectPtr<USceneComponent> DoorRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	TObjectPtr<UStaticMeshComponent> DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	TObjectPtr<UStaticMeshComponent> DoorPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	float OpenSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door")
	bool bAutoClose = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Door", meta = (EditCondition = "bAutoClose"))
	float AutoCloseDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Door|FX")
	TObjectPtr<USoundBase> OpenSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Door|FX")
	TObjectPtr<USoundBase> CloseSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Door|FX")
	TObjectPtr<USoundBase> LockedSound;

private:
	ELSDoorState DoorState = ELSDoorState::Closed;
	float CurrentAngle = 0.0f;
	float TargetAngle = 0.0f;

	FTimerHandle AutoCloseTimerHandle;
};
