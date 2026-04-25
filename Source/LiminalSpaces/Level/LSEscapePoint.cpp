#include "Level/LSEscapePoint.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Core/LSGameMode.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Engine/GameInstance.h"
#include "LiminalSpaces.h"

ALSEscapePoint::ALSEscapePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = TriggerVolume;

	EscapeFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EscapeFX"));
	EscapeFX->SetupAttachment(RootComponent);
	EscapeFX->bAutoActivate = false;
}

void ALSEscapePoint::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ALSEscapePoint::OnPlayerOverlap);

	if (bStartActive)
	{
		ActivateEscape();
	}
}

void ALSEscapePoint::ActivateEscape()
{
	bIsActive = true;

	if (EscapeFX)
	{
		EscapeFX->Activate();
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("Escape point activated"));
}

void ALSEscapePoint::DeactivateEscape()
{
	bIsActive = false;

	if (EscapeFX)
	{
		EscapeFX->Deactivate();
	}
}

void ALSEscapePoint::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive || bHasBeenTriggered)
	{
		return;
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (!PlayerCharacter || !PlayerCharacter->IsPlayerControlled())
	{
		return;
	}

	// Gate on clue collection — escape locks until enough memories are recovered
	ULSNarrativeSubsystem* Narr = nullptr;
	if (UGameInstance* GI = GetGameInstance())
	{
		Narr = GI->GetSubsystem<ULSNarrativeSubsystem>();
	}

	if (Narr && !Narr->IsEscapeUnlocked())
	{
		// Inform the player they need more clues, but allow the trigger to fire again later
		const int32 Have = Narr->GetCluesCollectedCount();
		const int32 Need = Narr->GetCluesRequiredToEscape();

		// Don't permanently lock the trigger; let them try again after picking up clues
		Narr->RequestLine(FName("escape_locked"));

		UE_LOG(LogLiminalSpaces, Log, TEXT("Escape blocked: %d/%d memories"), Have, Need);
		return;
	}

	bHasBeenTriggered = true;

	UE_LOG(LogLiminalSpaces, Log, TEXT("Player reached escape point!"));

	if (Narr)
	{
		Narr->RequestLine(FName("escape_open"));
	}

	if (EscapeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EscapeSound, GetActorLocation());
	}

	// Notify game mode
	APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
	if (PC)
	{
		if (ALSGameMode* GM = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnEscapeReached(PC);
		}
	}
}
