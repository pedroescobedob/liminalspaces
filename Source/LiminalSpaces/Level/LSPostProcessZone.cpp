#include "Level/LSPostProcessZone.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"

ALSPostProcessZone::ALSPostProcessZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	Volume->SetBoxExtent(FVector(500.0f, 500.0f, 300.0f));
	Volume->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = Volume;

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(RootComponent);
	PostProcess->bUnbound = false;
}

void ALSPostProcessZone::BeginPlay()
{
	Super::BeginPlay();
	ApplyPreset();
}

void ALSPostProcessZone::SetPreset(ELSPostProcessPreset NewPreset)
{
	Preset = NewPreset;
	ApplyPreset();
}

void ALSPostProcessZone::SetIntensity(float Intensity)
{
	EffectIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
	PostProcess->BlendWeight = EffectIntensity;
}

void ALSPostProcessZone::ApplyPreset()
{
	FPostProcessSettings& Settings = PostProcess->Settings;

	// Reset
	Settings = FPostProcessSettings();
	PostProcess->BlendWeight = EffectIntensity;

	switch (Preset)
	{
	case ELSPostProcessPreset::LiminalSubtle:
		Settings.bOverride_VignetteIntensity = true;
		Settings.VignetteIntensity = 0.6f;
		Settings.bOverride_ColorSaturation = true;
		Settings.ColorSaturation = FVector4(0.85f, 0.85f, 0.85f, 1.0f);
		Settings.bOverride_FilmGrainIntensity = true;
		Settings.FilmGrainIntensity = 0.15f;
		Settings.bOverride_SceneFringeIntensity = true;
		Settings.SceneFringeIntensity = 0.3f;
		break;

	case ELSPostProcessPreset::HorrorHeavy:
		Settings.bOverride_VignetteIntensity = true;
		Settings.VignetteIntensity = 1.0f;
		Settings.bOverride_ColorSaturation = true;
		Settings.ColorSaturation = FVector4(0.6f, 0.6f, 0.6f, 1.0f);
		Settings.bOverride_FilmGrainIntensity = true;
		Settings.FilmGrainIntensity = 0.4f;
		Settings.bOverride_SceneFringeIntensity = true;
		Settings.SceneFringeIntensity = 1.5f;
		Settings.bOverride_ColorContrast = true;
		Settings.ColorContrast = FVector4(1.2f, 1.2f, 1.2f, 1.0f);
		break;

	case ELSPostProcessPreset::SciFi:
		Settings.bOverride_VignetteIntensity = true;
		Settings.VignetteIntensity = 0.3f;
		Settings.bOverride_ColorGain = true;
		Settings.ColorGain = FVector4(0.9f, 0.95f, 1.1f, 1.0f);
		Settings.bOverride_BloomIntensity = true;
		Settings.BloomIntensity = 1.5f;
		Settings.bOverride_SceneFringeIntensity = true;
		Settings.SceneFringeIntensity = 0.5f;
		break;

	case ELSPostProcessPreset::Danger:
		Settings.bOverride_VignetteIntensity = true;
		Settings.VignetteIntensity = 1.2f;
		Settings.bOverride_ColorGain = true;
		Settings.ColorGain = FVector4(1.2f, 0.8f, 0.8f, 1.0f);
		Settings.bOverride_ColorContrast = true;
		Settings.ColorContrast = FVector4(1.3f, 1.3f, 1.3f, 1.0f);
		Settings.bOverride_ColorSaturation = true;
		Settings.ColorSaturation = FVector4(1.2f, 0.7f, 0.7f, 1.0f);
		break;

	case ELSPostProcessPreset::Dreamlike:
		Settings.bOverride_VignetteIntensity = true;
		Settings.VignetteIntensity = 0.4f;
		Settings.bOverride_ColorGain = true;
		Settings.ColorGain = FVector4(1.05f, 1.0f, 0.95f, 1.0f);
		Settings.bOverride_BloomIntensity = true;
		Settings.BloomIntensity = 2.5f;
		Settings.bOverride_ColorSaturation = true;
		Settings.ColorSaturation = FVector4(0.9f, 0.9f, 0.9f, 1.0f);
		break;

	case ELSPostProcessPreset::None:
	default:
		break;
	}
}
