#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LSInteractableBase.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULSInteractable : public UInterface
{
	GENERATED_BODY()
};

class LIMINALSPACES_API ILSInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LiminalSpaces|Interaction")
	bool CanInteract(ACharacter* InteractingCharacter) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LiminalSpaces|Interaction")
	void Interact(ACharacter* InteractingCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LiminalSpaces|Interaction")
	FText GetInteractionPrompt() const;
};
