#include "Enemy/LSEnemyBase.h"
#include "Combat/LSHealthComponent.h"
#include "Core/LSGameMode.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "LiminalSpaces.h"

ALSEnemyBase::ALSEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<ULSHealthComponent>(TEXT("HealthComponent"));

	// AI movement defaults
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

	bUseControllerRotationYaw = false;

	// Enable AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ALSEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ALSEnemyBase::OnDeath);
	}
}

float ALSEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && !HealthComponent->IsDead())
	{
		HealthComponent->ApplyDamage(ActualDamage);

		// Alert the enemy if not already aggressive
		if (CurrentState == ELSEnemyState::Idle || CurrentState == ELSEnemyState::Patrolling)
		{
			SetEnemyState(ELSEnemyState::Alert);
		}
	}

	return ActualDamage;
}

void ALSEnemyBase::SetEnemyState(ELSEnemyState NewState)
{
	CurrentState = NewState;
}

void ALSEnemyBase::PerformAttack()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = CurrentTime;

	if (AttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}

	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}

	// Damage trace in front of enemy
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * AttackRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50.0f), QueryParams))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			FPointDamageEvent DamageEvent(AttackDamage, HitResult, GetActorForwardVector(), nullptr);
			HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}
}

void ALSEnemyBase::OnDeath()
{
	SetEnemyState(ELSEnemyState::Dead);

	UE_LOG(LogLiminalSpaces, Log, TEXT("Enemy %s died"), *EnemyDisplayName);

	// Disable collision and movement
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// Play death effects
	if (DeathMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(DeathMontage);
		}
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathFX, GetActorLocation());
	}

	// Notify game mode
	if (ALSGameMode* GM = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnEnemyDefeated();
	}

	// Destroy after delay
	SetLifeSpan(DeathDestroyDelay);
}
