#include "Enemy/LSEnemySpawnPoint.h"
#include "Enemy/LSEnemyBase.h"
#include "Combat/LSHealthComponent.h"
#include "LiminalSpaces.h"

ALSEnemySpawnPoint::ALSEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	// Billboard for editor visibility
	bIsEditorOnlyActor = false;
#endif
}

bool ALSEnemySpawnPoint::TrySpawnEnemy()
{
	if (!EnemyClass)
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("EnemySpawnPoint %s has no EnemyClass set"), *GetName());
		return false;
	}

	if (FMath::FRand() > SpawnProbability)
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedEnemy = GetWorld()->SpawnActor<ALSEnemyBase>(EnemyClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	if (SpawnedEnemy)
	{
		if (ULSHealthComponent* HealthComp = SpawnedEnemy->GetHealthComponent())
		{
			HealthComp->OnDeath.AddDynamic(this, &ALSEnemySpawnPoint::OnSpawnedEnemyDeath);
		}

		UE_LOG(LogLiminalSpaces, Log, TEXT("Spawned enemy %s at %s"), *SpawnedEnemy->GetName(), *GetActorLocation().ToString());
		return true;
	}

	return false;
}

void ALSEnemySpawnPoint::RespawnEnemy()
{
	SpawnedEnemy = nullptr;
	TrySpawnEnemy();
}

void ALSEnemySpawnPoint::OnSpawnedEnemyDeath()
{
	if (bRespawnOnDeath)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALSEnemySpawnPoint::RespawnEnemy, RespawnDelay, false);
	}
}
