#include "Core/LSPlayerState.h"

ALSPlayerState::ALSPlayerState()
{
}

void ALSPlayerState::IncrementMapsCompleted()
{
	PlayerStats.MapsCompleted++;
}

void ALSPlayerState::IncrementEnemiesDefeated()
{
	PlayerStats.EnemiesDefeated++;
}

void ALSPlayerState::UpdateBestTime(float CompletionTime)
{
	if (PlayerStats.BestCompletionTime <= 0.0f || CompletionTime < PlayerStats.BestCompletionTime)
	{
		PlayerStats.BestCompletionTime = CompletionTime;
	}
}

void ALSPlayerState::SetAuthUserID(const FString& InUserID)
{
	AuthUserID = InUserID;
}
