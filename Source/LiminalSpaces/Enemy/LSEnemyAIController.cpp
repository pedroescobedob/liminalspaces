#include "Enemy/LSEnemyAIController.h"
#include "Enemy/LSEnemyBase.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/GameInstance.h"
#include "LiminalSpaces.h"

ALSEnemyAIController::ALSEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Perception
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	// Sight configuration
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = SightAngle;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerception->ConfigureSense(*SightConfig);

	// Hearing configuration
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->SetMaxAge(3.0f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerception->ConfigureSense(*HearingConfig);

	AIPerception->SetDominantSense(UAISense_Sight::StaticClass());
}

void ALSEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ALSEnemyAIController::OnPerceptionUpdated);
	}
}

void ALSEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ALSEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBehavior(DeltaTime);
}

void ALSEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// Detected a player
		APawn* DetectedPawn = Cast<APawn>(Actor);
		if (DetectedPawn && DetectedPawn->IsPlayerControlled())
		{
			SetTargetActor(Actor);
		}
	}
	else
	{
		// Lost sight
		if (Actor == TargetActor.Get())
		{
			ClearTarget();
		}
	}
}

void ALSEnemyAIController::SetTargetActor(AActor* InTarget)
{
	const bool bWasUntargeted = !TargetActor.IsValid();
	TargetActor = InTarget;

	if (ALSEnemyBase* Enemy = Cast<ALSEnemyBase>(GetPawn()))
	{
		if (Enemy->GetEnemyState() == ELSEnemyState::Idle || Enemy->GetEnemyState() == ELSEnemyState::Patrolling)
		{
			Enemy->SetEnemyState(ELSEnemyState::Chasing);
		}
	}

	if (bWasUntargeted)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
				{
					Narr->NotifyEnemyAlerted();
				}
			}
		}
	}
}

void ALSEnemyAIController::ClearTarget()
{
	const bool bWasTargeted = TargetActor.IsValid();
	TargetActor = nullptr;

	if (ALSEnemyBase* Enemy = Cast<ALSEnemyBase>(GetPawn()))
	{
		if (Enemy->GetEnemyState() != ELSEnemyState::Dead)
		{
			Enemy->SetEnemyState(ELSEnemyState::Patrolling);
		}
	}

	if (bWasTargeted)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
				{
					Narr->NotifyEnemyLost();
				}
			}
		}
	}
}

void ALSEnemyAIController::UpdateBehavior(float DeltaTime)
{
	ALSEnemyBase* Enemy = Cast<ALSEnemyBase>(GetPawn());
	if (!Enemy || Enemy->GetEnemyState() == ELSEnemyState::Dead)
	{
		return;
	}

	if (HasTarget())
	{
		float DistToTarget = FVector::Dist(Enemy->GetActorLocation(), TargetActor->GetActorLocation());

		if (DistToTarget <= AttackRange)
		{
			// In attack range - attack
			StopMovement();
			Enemy->SetEnemyState(ELSEnemyState::Attacking);
			Enemy->PerformAttack();
		}
		else
		{
			// Chase the target
			Enemy->SetEnemyState(ELSEnemyState::Chasing);
			MoveToActor(TargetActor.Get(), AttackRange * 0.8f);
		}
	}
	else
	{
		// No target - patrol or idle
		if (Enemy->GetEnemyState() == ELSEnemyState::Chasing || Enemy->GetEnemyState() == ELSEnemyState::Attacking)
		{
			Enemy->SetEnemyState(ELSEnemyState::Patrolling);
		}

		if (GetMoveStatus() == EPathFollowingStatus::Idle && Enemy->GetEnemyState() == ELSEnemyState::Patrolling)
		{
			// Random patrol point
			FNavLocation RandomLocation;
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (NavSys && NavSys->GetRandomPointInNavigableRadius(Enemy->GetActorLocation(), 800.0f, RandomLocation))
			{
				MoveToLocation(RandomLocation.Location);
			}
		}
	}
}
