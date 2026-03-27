#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSPatrolComponent.generated.h"

UCLASS(ClassGroup = (LiminalSpaces), meta = (BlueprintSpawnableComponent))
class LIMINALSPACES_API ULSPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULSPatrolComponent();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Patrol")
	FVector GetCurrentPatrolPoint() const;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Patrol")
	FVector GetNextPatrolPoint();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Patrol")
	void ResetPatrol();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Patrol")
	bool HasPatrolPoints() const { return PatrolPoints.Num() > 0; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Patrol")
	int32 GetCurrentIndex() const { return CurrentPointIndex; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Patrol")
	float GetWaitTime() const { return WaitTimeAtPoint; }

protected:
	// Patrol waypoints in world space. If empty, the AI will use random navigation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Patrol", meta = (MakeEditWidget = true))
	TArray<FVector> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Patrol")
	bool bLoopPatrol = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Patrol")
	bool bReverseAtEnd = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Patrol")
	float WaitTimeAtPoint = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Patrol")
	float PatrolSpeed = 150.0f;

private:
	int32 CurrentPointIndex = 0;
	int32 Direction = 1;
};
