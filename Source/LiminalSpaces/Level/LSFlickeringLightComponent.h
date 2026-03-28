#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSFlickeringLightComponent.generated.h"

class UPointLightComponent;
class UAudioComponent;
class USoundBase;

UENUM(BlueprintType)
enum class ELSFlickerPattern : uint8
{
	// Standard fluorescent flicker
	Fluorescent,
	// Occasional random dropout
	Dropout,
	// Slow pulse
	Pulse,
	// Erratic horror flicker
	Horror,
	// Constant (no flicker)
	Constant
};

UCLASS(ClassGroup = (LiminalSpaces), meta = (BlueprintSpawnableComponent))
class LIMINALSPACES_API ULSFlickeringLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULSFlickeringLightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Light")
	void SetFlickerEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Light")
	void SetFlickerPattern(ELSFlickerPattern NewPattern);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Light")
	void SetTargetLight(UPointLightComponent* Light);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light")
	ELSFlickerPattern FlickerPattern = ELSFlickerPattern::Fluorescent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light")
	float BaseIntensity = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light", meta = (ClampMin = "0.1", ClampMax = "20.0"))
	float FlickerSpeed = 3.0f;

	// Chance per second of a full blackout
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BlackoutChance = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light")
	float BlackoutDuration = 0.15f;

	// Min intensity as percentage of base
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinIntensityPercent = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light")
	FLinearColor FlickerColor = FLinearColor(0.85f, 0.95f, 1.0f, 1.0f);

	// Optional buzzing sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light|Audio")
	TObjectPtr<USoundBase> BuzzSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Light|Audio")
	float BuzzVolume = 0.3f;

private:
	UPROPERTY()
	TObjectPtr<UPointLightComponent> TargetLight;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BuzzAudioComponent;

	float ElapsedTime = 0.0f;
	bool bIsBlackout = false;
	float BlackoutTimer = 0.0f;
	bool bFlickerEnabled = true;

	float CalculateFluorescentFlicker() const;
	float CalculateDropoutFlicker() const;
	float CalculatePulseFlicker() const;
	float CalculateHorrorFlicker() const;
};
