#include "Core/LSGameState.h"

ALSGameState::ALSGameState()
{
}

void ALSGameState::SetGameProgress(ELSGameState NewState)
{
	if (CurrentGameState != NewState)
	{
		CurrentGameState = NewState;
		OnGameProgressChanged.Broadcast(NewState);
	}
}

void ALSGameState::SetEnemiesRemaining(int32 Count)
{
	EnemiesRemaining = FMath::Max(0, Count);
}
