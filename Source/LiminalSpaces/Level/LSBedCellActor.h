#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "LSBedCellActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;

UCLASS()
class LIMINALSPACES_API ALSBedCellActor : public APlayerStart
{
	GENERATED_BODY()

public:
	ALSBedCellActor(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	// Room trigger volume to detect player leaving the bed cell
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell")
	TObjectPtr<UBoxComponent> RoomTrigger;

	// Ambient light for the bed cell
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell")
	TObjectPtr<UPointLightComponent> AmbientLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell")
	FLinearColor LightColor = FLinearColor(0.8f, 0.9f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell")
	float LightIntensity = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell")
	bool bFlickerLight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|BedCell", meta = (EditCondition = "bFlickerLight"))
	float FlickerFrequency = 2.0f;

private:
	UFUNCTION()
	void OnPlayerExitBedCell(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TickLightFlicker();

	FTimerHandle FlickerTimerHandle;
};
