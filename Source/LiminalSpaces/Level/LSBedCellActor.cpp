#include "Level/LSBedCellActor.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"
#include "LiminalSpaces.h"

ALSBedCellActor::ALSBedCellActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// Room trigger
	RoomTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomTrigger"));
	RoomTrigger->SetupAttachment(RootComponent);
	RoomTrigger->SetBoxExtent(FVector(300.0f, 300.0f, 200.0f));
	RoomTrigger->SetCollisionProfileName(TEXT("Trigger"));

	// Ambient light
	AmbientLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AmbientLight"));
	AmbientLight->SetupAttachment(RootComponent);
	AmbientLight->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	AmbientLight->SetIntensity(LightIntensity);
	AmbientLight->SetLightColor(LightColor);
	AmbientLight->SetAttenuationRadius(500.0f);
}

void ALSBedCellActor::BeginPlay()
{
	Super::BeginPlay();

	RoomTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSBedCellActor::OnPlayerExitBedCell);

	if (bFlickerLight)
	{
		GetWorldTimerManager().SetTimer(FlickerTimerHandle, this, &ALSBedCellActor::TickLightFlicker, 1.0f / FlickerFrequency, true);
	}
}

void ALSBedCellActor::OnPlayerExitBedCell(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ACharacter>(OtherActor) && Cast<ACharacter>(OtherActor)->IsPlayerControlled())
	{
		UE_LOG(LogLiminalSpaces, Log, TEXT("Player left the bed cell - exploration begins"));
	}
}

void ALSBedCellActor::TickLightFlicker()
{
	if (AmbientLight)
	{
		float Flicker = FMath::FRandRange(0.7f, 1.0f);
		AmbientLight->SetIntensity(LightIntensity * Flicker);

		// Occasional full flicker off
		if (FMath::FRand() < 0.05f)
		{
			AmbientLight->SetIntensity(LightIntensity * 0.1f);
		}
	}
}
