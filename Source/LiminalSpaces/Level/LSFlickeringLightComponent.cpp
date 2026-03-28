#include "Level/LSFlickeringLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ULSFlickeringLightComponent::ULSFlickeringLightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.016f; // ~60Hz for smooth flicker
}

void ULSFlickeringLightComponent::BeginPlay()
{
	Super::BeginPlay();

	// Auto-find a PointLightComponent on the owner if not set
	if (!TargetLight)
	{
		TargetLight = GetOwner()->FindComponentByClass<UPointLightComponent>();
	}

	if (TargetLight)
	{
		BaseIntensity = TargetLight->Intensity;
	}

	// Start buzz sound
	if (BuzzSound)
	{
		BuzzAudioComponent = UGameplayStatics::SpawnSoundAttached(
			BuzzSound,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			false,
			BuzzVolume,
			1.0f,
			0.0f,
			nullptr,
			nullptr,
			false
		);
		if (BuzzAudioComponent)
		{
			BuzzAudioComponent->bAutoDestroy = false;
		}
	}
}

void ULSFlickeringLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TargetLight || !bFlickerEnabled)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	// Handle blackout
	if (bIsBlackout)
	{
		BlackoutTimer -= DeltaTime;
		if (BlackoutTimer <= 0.0f)
		{
			bIsBlackout = false;
		}
		else
		{
			TargetLight->SetIntensity(BaseIntensity * 0.02f);
			return;
		}
	}

	// Random blackout chance
	if (FMath::FRand() < BlackoutChance * DeltaTime)
	{
		bIsBlackout = true;
		BlackoutTimer = BlackoutDuration * FMath::FRandRange(0.5f, 1.5f);
		TargetLight->SetIntensity(0.0f);
		return;
	}

	// Calculate intensity based on pattern
	float IntensityMultiplier = 1.0f;

	switch (FlickerPattern)
	{
	case ELSFlickerPattern::Fluorescent:
		IntensityMultiplier = CalculateFluorescentFlicker();
		break;
	case ELSFlickerPattern::Dropout:
		IntensityMultiplier = CalculateDropoutFlicker();
		break;
	case ELSFlickerPattern::Pulse:
		IntensityMultiplier = CalculatePulseFlicker();
		break;
	case ELSFlickerPattern::Horror:
		IntensityMultiplier = CalculateHorrorFlicker();
		break;
	case ELSFlickerPattern::Constant:
		IntensityMultiplier = 1.0f;
		break;
	}

	float FinalIntensity = BaseIntensity * FMath::Clamp(IntensityMultiplier, MinIntensityPercent, 1.0f);
	TargetLight->SetIntensity(FinalIntensity);

	// Modulate buzz volume with flicker
	if (BuzzAudioComponent)
	{
		BuzzAudioComponent->SetVolumeMultiplier(BuzzVolume * IntensityMultiplier);
	}
}

void ULSFlickeringLightComponent::SetFlickerEnabled(bool bEnabled)
{
	bFlickerEnabled = bEnabled;
	if (!bEnabled && TargetLight)
	{
		TargetLight->SetIntensity(BaseIntensity);
	}
}

void ULSFlickeringLightComponent::SetFlickerPattern(ELSFlickerPattern NewPattern)
{
	FlickerPattern = NewPattern;
}

void ULSFlickeringLightComponent::SetTargetLight(UPointLightComponent* Light)
{
	TargetLight = Light;
	if (TargetLight)
	{
		BaseIntensity = TargetLight->Intensity;
	}
}

float ULSFlickeringLightComponent::CalculateFluorescentFlicker() const
{
	// Simulate fluorescent light with multiple sine waves
	float Flicker = 0.0f;
	Flicker += FMath::Sin(ElapsedTime * FlickerSpeed * 6.2832f) * 0.08f;
	Flicker += FMath::Sin(ElapsedTime * FlickerSpeed * 2.1f * 6.2832f) * 0.05f;
	Flicker += FMath::Sin(ElapsedTime * FlickerSpeed * 0.3f * 6.2832f) * 0.12f;
	// Add small random noise
	Flicker += FMath::FRandRange(-0.03f, 0.03f);
	return 1.0f + Flicker;
}

float ULSFlickeringLightComponent::CalculateDropoutFlicker() const
{
	// Mostly steady with occasional drops
	float Random = FMath::FRand();
	if (Random < 0.02f)
	{
		return FMath::FRandRange(0.1f, 0.4f);
	}
	return 1.0f + FMath::FRandRange(-0.02f, 0.02f);
}

float ULSFlickeringLightComponent::CalculatePulseFlicker() const
{
	// Slow, breathing pulse
	float Pulse = FMath::Sin(ElapsedTime * FlickerSpeed * 0.5f * 6.2832f);
	return 0.7f + Pulse * 0.3f;
}

float ULSFlickeringLightComponent::CalculateHorrorFlicker() const
{
	// Erratic, unsettling pattern
	float Base = FMath::Sin(ElapsedTime * FlickerSpeed * 6.2832f) * 0.3f;
	float Erratic = FMath::Sin(ElapsedTime * FlickerSpeed * 7.3f) * FMath::Sin(ElapsedTime * FlickerSpeed * 3.1f);
	float Noise = FMath::FRandRange(-0.15f, 0.15f);

	float Combined = 0.6f + Base + Erratic * 0.2f + Noise;

	// Occasional rapid strobe
	if (FMath::FRand() < 0.01f)
	{
		Combined = FMath::FRand() < 0.5f ? 0.05f : 1.2f;
	}

	return Combined;
}
