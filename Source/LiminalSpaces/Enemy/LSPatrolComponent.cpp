#include "Enemy/LSPatrolComponent.h"

ULSPatrolComponent::ULSPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector ULSPatrolComponent::GetCurrentPatrolPoint() const
{
	if (PatrolPoints.IsValidIndex(CurrentPointIndex))
	{
		// Convert from local to world space
		return GetOwner()->GetActorTransform().TransformPosition(PatrolPoints[CurrentPointIndex]);
	}
	return GetOwner()->GetActorLocation();
}

FVector ULSPatrolComponent::GetNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		return GetOwner()->GetActorLocation();
	}

	CurrentPointIndex += Direction;

	if (bReverseAtEnd)
	{
		if (CurrentPointIndex >= PatrolPoints.Num())
		{
			CurrentPointIndex = PatrolPoints.Num() - 2;
			Direction = -1;
		}
		else if (CurrentPointIndex < 0)
		{
			CurrentPointIndex = 1;
			Direction = 1;
		}
	}
	else if (bLoopPatrol)
	{
		CurrentPointIndex = CurrentPointIndex % PatrolPoints.Num();
	}
	else
	{
		CurrentPointIndex = FMath::Clamp(CurrentPointIndex, 0, PatrolPoints.Num() - 1);
	}

	return GetCurrentPatrolPoint();
}

void ULSPatrolComponent::ResetPatrol()
{
	CurrentPointIndex = 0;
	Direction = 1;
}
