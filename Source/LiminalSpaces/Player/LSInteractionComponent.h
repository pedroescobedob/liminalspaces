#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSInteractionComponent.generated.h"

class ILSInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChanged, AActor*, NewTarget);

UCLASS(ClassGroup = (LiminalSpaces), meta = (BlueprintSpawnableComponent))
class LIMINALSPACES_API ULSInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULSInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Interaction")
	void TryInteract();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Interaction")
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Interaction")
	FText GetInteractionPrompt() const;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Interaction")
	bool HasInteractionTarget() const { return CurrentTarget.IsValid(); }

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Events")
	FOnInteractionTargetChanged OnInteractionTargetChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Interaction")
	float InteractionRange = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Interaction")
	float TraceRadius = 30.0f;

private:
	void PerformInteractionTrace();

	TWeakObjectPtr<AActor> CurrentTarget;
};
