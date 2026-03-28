#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSEscapePoint.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class USoundBase;

UCLASS()
class LIMINALSPACES_API ALSEscapePoint : public AActor
{
	GENERATED_BODY()

public:
	ALSEscapePoint();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Escape")
	void ActivateEscape();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Escape")
	void DeactivateEscape();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Escape")
	bool IsActive() const { return bIsActive; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Escape")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Escape")
	TObjectPtr<UNiagaraComponent> EscapeFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Escape")
	TObjectPtr<USoundBase> EscapeSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Escape")
	bool bStartActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Escape")
	FText EscapeText = FText::FromString(TEXT("ESCAPE"));

private:
	bool bIsActive = false;
	bool bHasBeenTriggered = false;
};
