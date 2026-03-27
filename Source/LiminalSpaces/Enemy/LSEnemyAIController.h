#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "LSEnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class LIMINALSPACES_API ALSEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ALSEnemyAIController();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|AI")
	AActor* GetTargetActor() const { return TargetActor.Get(); }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|AI")
	bool HasTarget() const { return TargetActor.IsValid(); }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|AI")
	void SetTargetActor(AActor* InTarget);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|AI")
	void ClearTarget();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	float SightRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	float LoseSightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	float SightAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	float HearingRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|AI")
	float AttackRange = 200.0f;

private:
	void UpdateBehavior(float DeltaTime);

	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
};
