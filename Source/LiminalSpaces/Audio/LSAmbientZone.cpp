#include "Audio/LSAmbientZone.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"

ALSAmbientZone::ALSAmbientZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneVolume"));
	ZoneVolume->SetBoxExtent(FVector(500.0f, 500.0f, 300.0f));
	ZoneVolume->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = ZoneVolume;

	AmbientAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientAudio"));
	AmbientAudio->SetupAttachment(RootComponent);
	AmbientAudio->bAutoActivate = false;
}

void ALSAmbientZone::BeginPlay()
{
	Super::BeginPlay();

	if (AmbientSound)
	{
		AmbientAudio->SetSound(AmbientSound);
		AmbientAudio->SetVolumeMultiplier(0.0f);
	}

	ZoneVolume->OnComponentBeginOverlap.AddDynamic(this, &ALSAmbientZone::OnPlayerEnter);
	ZoneVolume->OnComponentEndOverlap.AddDynamic(this, &ALSAmbientZone::OnPlayerExit);
}

void ALSAmbientZone::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (Character->IsPlayerControlled())
		{
			AmbientAudio->Play();
			AmbientAudio->FadeIn(FadeInDuration, Volume);
		}
	}
}

void ALSAmbientZone::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (Character->IsPlayerControlled())
		{
			AmbientAudio->FadeOut(FadeOutDuration, 0.0f);
		}
	}
}
