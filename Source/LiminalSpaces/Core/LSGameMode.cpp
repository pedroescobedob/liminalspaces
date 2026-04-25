#include "Core/LSGameMode.h"
#include "Core/LSGameState.h"
#include "Core/LSPlayerController.h"
#include "Player/LSCharacter.h"
#include "Level/LSBedCellActor.h"
#include "Level/LSEscapePoint.h"
#include "Level/LSBackroomsGenerator.h"
#include "Enemy/LSEnemySpawnPoint.h"
#include "Data/LSMapDefinition.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "UI/LSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "LiminalSpaces.h"

ALSGameMode::ALSGameMode()
{
	DefaultPawnClass = ALSCharacter::StaticClass();
	PlayerControllerClass = ALSPlayerController::StaticClass();
	GameStateClass = ALSGameState::StaticClass();
	HUDClass = ALSHUD::StaticClass();
}

void ALSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	MapStartTime = GetWorld()->GetTimeSeconds();

	// Reset narrative state so a new playthrough starts clean
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			Narr->ResetForNewGame();
		}
	}

	// Spawn a backrooms generator if no level geometry exists
	bool bHasGenerator = false;
	for (TActorIterator<ALSBackroomsGenerator> It(GetWorld()); It; ++It)
	{
		bHasGenerator = true;
		break;
	}

	if (!bHasGenerator)
	{
		UE_LOG(LogLiminalSpaces, Log, TEXT("No level generator found, generating backrooms..."));
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ALSBackroomsGenerator* Generator = GetWorld()->SpawnActor<ALSBackroomsGenerator>(ALSBackroomsGenerator::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Generator)
		{
			// Generate now so PlayerStart is in place before player spawns
			Generator->GenerateLevel();
		}
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("InitGame: %s"), *MapName);
}

void ALSGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	SpawnEnemies();
}

AActor* ALSGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	// Look for a BedCellActor first as the preferred spawn point
	for (TActorIterator<ALSBedCellActor> It(GetWorld()); It; ++It)
	{
		return *It;
	}

	// Fall back to default PlayerStart
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void ALSGameMode::OnEscapeReached(APlayerController* Player)
{
	float CompletionTime = GetElapsedTime();
	UE_LOG(LogLiminalSpaces, Log, TEXT("Player escaped in %.1f seconds"), CompletionTime);

	if (ALSGameState* GS = GetGameState<ALSGameState>())
	{
		GS->SetGameProgress(ELSGameState::Escaped);
	}

	// Show win screen via HUD; gather clue count from narrative subsystem
	int32 CluesCollected = 0;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			CluesCollected = Narr->GetCluesCollectedCount();
		}
	}

	if (Player)
	{
		if (ALSHUD* HUD = Cast<ALSHUD>(Player->GetHUD()))
		{
			HUD->ShowWinScreen(CompletionTime, CluesCollected);
		}
	}

	OnPlayerEscaped.Broadcast(Player);
}

void ALSGameMode::OnEnemyDefeated()
{
	EnemiesRemaining = FMath::Max(0, EnemiesRemaining - 1);

	if (EnemiesRemaining <= 0)
	{
		OnAllEnemiesDefeated.Broadcast();
	}
}

void ALSGameMode::RestartMap(APlayerController* Player)
{
	if (Player)
	{
		GetWorld()->ServerTravel(TEXT("?Restart"), false);
	}
}

float ALSGameMode::GetElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - MapStartTime;
}

void ALSGameMode::SpawnEnemies()
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSEnemySpawnPoint::StaticClass(), SpawnPoints);

	EnemiesRemaining = 0;
	TotalEnemies = 0;

	for (AActor* Actor : SpawnPoints)
	{
		if (ALSEnemySpawnPoint* SpawnPoint = Cast<ALSEnemySpawnPoint>(Actor))
		{
			if (SpawnPoint->TrySpawnEnemy())
			{
				EnemiesRemaining++;
				TotalEnemies++;
			}
		}
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("Spawned %d enemies"), TotalEnemies);
}
