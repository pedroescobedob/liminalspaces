#include "Narrative/LSNarrativeTrigger.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine/GameInstance.h"

ALSNarrativeTrigger::ALSNarrativeTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetBoxExtent(FVector(150.0f, 150.0f, 200.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = TriggerVolume;
}

void ALSNarrativeTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ALSNarrativeTrigger::HandlePlayerOverlap);
}

void ALSNarrativeTrigger::SetExtents(FVector InExtents)
{
	if (TriggerVolume)
	{
		TriggerVolume->SetBoxExtent(InExtents);
	}
}

void ALSNarrativeTrigger::HandlePlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bOneShot && bHasFired)
	{
		return;
	}

	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (!Player || !Player->IsPlayerControlled())
	{
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			Narr->RequestLine(LineID);
			bHasFired = true;
		}
	}
}
