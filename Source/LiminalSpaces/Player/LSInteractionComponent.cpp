#include "Player/LSInteractionComponent.h"
#include "Level/LSInteractableBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

ULSInteractionComponent::ULSInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Trace every 100ms
}

void ULSInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULSInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformInteractionTrace();
}

void ULSInteractionComponent::PerformInteractionTrace()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		CameraLocation,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);

	AActor* NewTarget = nullptr;

	if (bHit && HitResult.GetActor())
	{
		if (HitResult.GetActor()->Implements<ULSInteractable>())
		{
			if (ILSInteractable::Execute_CanInteract(HitResult.GetActor(), OwnerCharacter))
			{
				NewTarget = HitResult.GetActor();
			}
		}
	}

	if (NewTarget != CurrentTarget.Get())
	{
		CurrentTarget = NewTarget;
		OnInteractionTargetChanged.Broadcast(NewTarget);
	}
}

void ULSInteractionComponent::TryInteract()
{
	if (CurrentTarget.IsValid())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OwnerCharacter)
		{
			ILSInteractable::Execute_Interact(CurrentTarget.Get(), OwnerCharacter);
		}
	}
}

FText ULSInteractionComponent::GetInteractionPrompt() const
{
	if (CurrentTarget.IsValid())
	{
		return ILSInteractable::Execute_GetInteractionPrompt(CurrentTarget.Get());
	}
	return FText::GetEmpty();
}
