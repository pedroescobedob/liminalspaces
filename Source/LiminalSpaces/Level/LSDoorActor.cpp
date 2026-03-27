#include "Level/LSDoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiminalSpaces.h"

ALSDoorActor::ALSDoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	RootComponent = DoorRoot;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(DoorRoot);

	DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
	DoorPanel->SetupAttachment(DoorRoot);
}

void ALSDoorActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALSDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DoorState == ELSDoorState::Opening || DoorState == ELSDoorState::Closing)
	{
		CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, OpenSpeed);

		if (DoorPanel)
		{
			FRotator NewRotation = FRotator(0.0f, CurrentAngle, 0.0f);
			DoorPanel->SetRelativeRotation(NewRotation);
		}

		if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 0.5f))
		{
			CurrentAngle = TargetAngle;
			DoorState = (TargetAngle > 0.0f) ? ELSDoorState::Open : ELSDoorState::Closed;

			if (DoorState == ELSDoorState::Open && bAutoClose)
			{
				GetWorldTimerManager().SetTimer(AutoCloseTimerHandle, this, &ALSDoorActor::CloseDoor, AutoCloseDelay, false);
			}
		}
	}
}

bool ALSDoorActor::CanInteract_Implementation(ACharacter* InteractingCharacter) const
{
	return DoorState != ELSDoorState::Opening && DoorState != ELSDoorState::Closing;
}

void ALSDoorActor::Interact_Implementation(ACharacter* InteractingCharacter)
{
	if (bIsLocked)
	{
		if (LockedSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LockedSound, GetActorLocation());
		}
		return;
	}

	ToggleDoor();
}

FText ALSDoorActor::GetInteractionPrompt_Implementation() const
{
	if (bIsLocked)
	{
		return FText::FromString(TEXT("Locked"));
	}

	return (DoorState == ELSDoorState::Closed)
		? FText::FromString(TEXT("Open Door"))
		: FText::FromString(TEXT("Close Door"));
}

void ALSDoorActor::OpenDoor()
{
	if (DoorState == ELSDoorState::Closed || DoorState == ELSDoorState::Closing)
	{
		DoorState = ELSDoorState::Opening;
		TargetAngle = OpenAngle;

		if (OpenSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}
	}
}

void ALSDoorActor::CloseDoor()
{
	if (DoorState == ELSDoorState::Open || DoorState == ELSDoorState::Opening)
	{
		DoorState = ELSDoorState::Closing;
		TargetAngle = 0.0f;

		if (CloseSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
		}
	}
}

void ALSDoorActor::ToggleDoor()
{
	if (DoorState == ELSDoorState::Closed)
	{
		OpenDoor();
	}
	else if (DoorState == ELSDoorState::Open)
	{
		CloseDoor();
	}
}
