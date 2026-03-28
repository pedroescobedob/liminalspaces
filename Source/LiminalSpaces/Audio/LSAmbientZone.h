#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSAmbientZone.generated.h"

class UBoxComponent;
class UAudioComponent;
class USoundBase;

UCLASS()
class LIMINALSPACES_API ALSAmbientZone : public AActor
{
	GENERATED_BODY()

public:
	ALSAmbientZone();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	TObjectPtr<UBoxComponent> ZoneVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	TObjectPtr<UAudioComponent> AmbientAudio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	TObjectPtr<USoundBase> AmbientSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	float FadeInDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Audio")
	float FadeOutDuration = 1.0f;
};
