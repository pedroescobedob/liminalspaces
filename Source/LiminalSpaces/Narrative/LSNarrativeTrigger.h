#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSNarrativeTrigger.generated.h"

class UBoxComponent;

/**
 * Volume that fires a single dialog line when the player walks through it.
 */
UCLASS()
class LIMINALSPACES_API ALSNarrativeTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALSNarrativeTrigger();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void SetLineID(FName InLineID) { LineID = InLineID; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void SetExtents(FVector InExtents);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Narrative")
	FName LineID;

	UPROPERTY(EditAnywhere, Category = "LiminalSpaces|Narrative")
	bool bOneShot = true;

private:
	bool bHasFired = false;
};
