#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSPostProcessZone.generated.h"

class UBoxComponent;
class UPostProcessComponent;

// Preset horror/liminal post-process effects
UENUM(BlueprintType)
enum class ELSPostProcessPreset : uint8
{
	// No effects
	None,
	// Subtle liminal unease: slight vignette, desaturated
	LiminalSubtle,
	// Heavy horror: chromatic aberration, film grain, vignette, desaturated
	HorrorHeavy,
	// Sci-fi: blue tint, bloom, slight lens flare
	SciFi,
	// Danger: red tint, high contrast, pulsing vignette
	Danger,
	// Dream: soft focus, warm tint, bloom
	Dreamlike
};

UCLASS()
class LIMINALSPACES_API ALSPostProcessZone : public AActor
{
	GENERATED_BODY()

public:
	ALSPostProcessZone();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|PostProcess")
	void SetPreset(ELSPostProcessPreset NewPreset);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|PostProcess")
	void SetIntensity(float Intensity);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	TObjectPtr<UPostProcessComponent> PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess")
	ELSPostProcessPreset Preset = ELSPostProcessPreset::LiminalSubtle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EffectIntensity = 1.0f;

private:
	void ApplyPreset();
};
