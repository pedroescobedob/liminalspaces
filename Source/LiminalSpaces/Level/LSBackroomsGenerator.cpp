#include "Level/LSBackroomsGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Level/LSEscapePoint.h"
#include "Level/LSDoorActor.h"
#include "Level/LSFlickeringLightComponent.h"
#include "Level/LSWeaponPickup.h"
#include "Narrative/LSClueNote.h"
#include "Narrative/LSNarrativeTrigger.h"
#include "Narrative/LSStoryContent.h"
#include "Combat/LSMeleeWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ALSBackroomsGenerator::ALSBackroomsGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create instanced mesh components
	FloorMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorMeshes"));
	FloorMeshes->SetupAttachment(RootComponent);
	FloorMeshes->SetMobility(EComponentMobility::Movable);

	CeilingMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CeilingMeshes"));
	CeilingMeshes->SetupAttachment(RootComponent);
	CeilingMeshes->SetMobility(EComponentMobility::Movable);

	WallMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallMeshes"));
	WallMeshes->SetupAttachment(RootComponent);
	WallMeshes->SetMobility(EComponentMobility::Movable);

	// Load cube mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		FloorMeshes->SetStaticMesh(CubeMesh.Object);
		CeilingMeshes->SetStaticMesh(CubeMesh.Object);
		WallMeshes->SetStaticMesh(CubeMesh.Object);
	}
}

void ALSBackroomsGenerator::BeginPlay()
{
	Super::BeginPlay();
	// Only generate if not already done (GameMode may call GenerateLevel early)
	if (FloorMeshes->GetInstanceCount() == 0)
	{
		GenerateLevel();
	}
}

void ALSBackroomsGenerator::GenerateLevel()
{
	if (RandomSeed == 0)
	{
		RandomSeed = FMath::RandRange(1, 999999);
	}
	FMath::RandInit(RandomSeed);
	UE_LOG(LogLiminalSpaces, Log, TEXT("Generating backrooms with seed: %d"), RandomSeed);

	CreateMaterials();
	GenerateMaze();
	SpawnGeometry();
	SpawnLighting();
	SpawnPlayerStart();
	SpawnEscapePoint();
	SpawnNarrativeContent();
}

void ALSBackroomsGenerator::CreateMaterials()
{
	UMaterial* BaseMat = UMaterial::GetDefaultMaterial(MD_Surface);

	// Dirty white hospital/hotel walls
	WallMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	WallMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.82f, 0.80f, 0.77f));
	WallMeshes->SetMaterial(0, WallMaterial);

	// Hospital linoleum floor - grayish beige
	FloorMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	FloorMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.45f, 0.43f, 0.40f));
	FloorMeshes->SetMaterial(0, FloorMaterial);

	// Off-white ceiling
	CeilingMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	CeilingMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.88f, 0.85f, 0.78f));
	CeilingMeshes->SetMaterial(0, CeilingMaterial);
}

void ALSBackroomsGenerator::GenerateMaze()
{
	// Backrooms layout: open floor plan with wall partitions creating rooms and corridors
	Grid.SetNum(GridSizeX);
	for (int32 X = 0; X < GridSizeX; X++)
	{
		Grid[X].SetNum(GridSizeY);
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			// Border is walls, everything inside is open
			Grid[X][Y] = (X > 0 && X < GridSizeX - 1 && Y > 0 && Y < GridSizeY - 1);
		}
	}

	// Add wall partitions to create rooms and corridors
	int32 NumPartitions = (GridSizeX + GridSizeY) / 2;
	for (int32 i = 0; i < NumPartitions; i++)
	{
		bool bHorizontal = FMath::FRand() < 0.5f;
		int32 Length = FMath::RandRange(3, 7);
		int32 StartX = FMath::RandRange(2, GridSizeX - 3);
		int32 StartY = FMath::RandRange(2, GridSizeY - 3);

		// Place wall segment
		for (int32 j = 0; j < Length; j++)
		{
			int32 WX = bHorizontal ? FMath::Clamp(StartX + j, 1, GridSizeX - 2) : StartX;
			int32 WY = bHorizontal ? StartY : FMath::Clamp(StartY + j, 1, GridSizeY - 2);
			Grid[WX][WY] = false;
		}

		// Cut 1-2 doorways in each wall so all areas stay connected
		int32 NumDoors = FMath::RandRange(1, 2);
		for (int32 d = 0; d < NumDoors; d++)
		{
			int32 DoorOffset = FMath::RandRange(0, Length - 1);
			int32 DX = bHorizontal ? FMath::Clamp(StartX + DoorOffset, 1, GridSizeX - 2) : StartX;
			int32 DY = bHorizontal ? StartY : FMath::Clamp(StartY + DoorOffset, 1, GridSizeY - 2);
			Grid[DX][DY] = true;
		}
	}

	// Add some scattered pillars for visual interest
	for (int32 X = 2; X < GridSizeX - 2; X += 3)
	{
		for (int32 Y = 2; Y < GridSizeY - 2; Y += 3)
		{
			if (FMath::FRand() < 0.2f)
			{
				Grid[X][Y] = false;
			}
		}
	}

	// Ensure start area and escape area are always open
	Grid[2][2] = true;
	Grid[3][2] = true;
	Grid[2][3] = true;
	Grid[3][3] = true;

	// Find the farthest passage from start for escape point
	int32 MaxDist = 0;
	EscapeLocation = FVector(2 * CellSize, 2 * CellSize, 50.0f);
	for (int32 X = 1; X < GridSizeX - 1; X++)
	{
		for (int32 Y = 1; Y < GridSizeY - 1; Y++)
		{
			if (Grid[X][Y])
			{
				int32 Dist = FMath::Abs(X - 2) + FMath::Abs(Y - 2);
				if (Dist > MaxDist)
				{
					MaxDist = Dist;
					EscapeLocation = FVector(X * CellSize, Y * CellSize, 50.0f);
				}
			}
		}
	}

	// Ensure escape area is open
	int32 EscapeGridX = FMath::RoundToInt(EscapeLocation.X / CellSize);
	int32 EscapeGridY = FMath::RoundToInt(EscapeLocation.Y / CellSize);
	Grid[EscapeGridX][EscapeGridY] = true;
}

void ALSBackroomsGenerator::CarveMaze(int32 X, int32 Y, TArray<TArray<bool>>& Visited)
{
	// No longer used - kept for compatibility
}

bool ALSBackroomsGenerator::IsPassage(int32 X, int32 Y) const
{
	if (X < 0 || X >= GridSizeX || Y < 0 || Y >= GridSizeY)
		return false;
	return Grid[X][Y];
}

void ALSBackroomsGenerator::SpawnGeometry()
{
	// The cube mesh is 100x100x100, centered at origin
	const float CubeBase = 100.0f;
	const float FloorThickness = 10.0f;
	const float CeilingThickness = 10.0f;

	for (int32 X = 0; X < GridSizeX; X++)
	{
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			if (!Grid[X][Y])
				continue;

			FVector CellCenter(X * CellSize, Y * CellSize, 0.0f);

			// Floor tile
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(0, 0, -FloorThickness / 2.0f));
				T.SetScale3D(FVector(CellSize / CubeBase, CellSize / CubeBase, FloorThickness / CubeBase));
				FloorMeshes->AddInstance(T);
			}

			// Ceiling tile
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(0, 0, WallHeight + CeilingThickness / 2.0f));
				T.SetScale3D(FVector(CellSize / CubeBase, CellSize / CubeBase, CeilingThickness / CubeBase));
				CeilingMeshes->AddInstance(T);
			}

			// Walls - check each side, add wall if neighbor is not a passage
			// North wall (positive Y)
			if (!IsPassage(X, Y + 1))
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(0, CellSize / 2.0f, WallHeight / 2.0f));
				T.SetScale3D(FVector(CellSize / CubeBase, WallThickness / CubeBase, WallHeight / CubeBase));
				WallMeshes->AddInstance(T);
			}
			// South wall (negative Y)
			if (!IsPassage(X, Y - 1))
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(0, -CellSize / 2.0f, WallHeight / 2.0f));
				T.SetScale3D(FVector(CellSize / CubeBase, WallThickness / CubeBase, WallHeight / CubeBase));
				WallMeshes->AddInstance(T);
			}
			// East wall (positive X)
			if (!IsPassage(X + 1, Y))
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(CellSize / 2.0f, 0, WallHeight / 2.0f));
				T.SetScale3D(FVector(WallThickness / CubeBase, CellSize / CubeBase, WallHeight / CubeBase));
				WallMeshes->AddInstance(T);
			}
			// West wall (negative X)
			if (!IsPassage(X - 1, Y))
			{
				FTransform T;
				T.SetLocation(CellCenter + FVector(-CellSize / 2.0f, 0, WallHeight / 2.0f));
				T.SetScale3D(FVector(WallThickness / CubeBase, CellSize / CubeBase, WallHeight / CubeBase));
				WallMeshes->AddInstance(T);
			}
		}
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("Generated geometry - Floors: %d, Ceilings: %d, Walls: %d"),
		FloorMeshes->GetInstanceCount(), CeilingMeshes->GetInstanceCount(), WallMeshes->GetInstanceCount());
}

void ALSBackroomsGenerator::SpawnLighting()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Add a dim skylight for baseline ambient illumination
	{
		AActor* SkyActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		if (SkyActor)
		{
			USceneComponent* SkyRoot = NewObject<USceneComponent>(SkyActor, TEXT("SkyRoot"));
			SkyActor->SetRootComponent(SkyRoot);
			SkyRoot->RegisterComponent();

			USkyLightComponent* SkyLight = NewObject<USkyLightComponent>(SkyActor);
			SkyLight->SetupAttachment(SkyRoot);
			SkyLight->SetMobility(EComponentMobility::Movable);
			SkyLight->SetIntensity(0.15f);
			SkyLight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
			SkyLight->RegisterComponent();
			SkyLight->RecaptureSky();
		}
	}

	for (int32 X = 0; X < GridSizeX; X++)
	{
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			if (!Grid[X][Y])
				continue;

			// Place a light every 2 cells for good coverage
			bool bPlaceLight = (X % 2 == 0 && Y % 2 == 0);
			if (!bPlaceLight)
				continue;

			FVector LightPos(X * CellSize, Y * CellSize, WallHeight - 20.0f);

			// Spawn a light actor
			AActor* LightActor = World->SpawnActor<AActor>(AActor::StaticClass(), LightPos, FRotator::ZeroRotator);
			if (!LightActor)
				continue;

			USceneComponent* LightRoot = NewObject<USceneComponent>(LightActor, TEXT("LightRoot"));
			LightActor->SetRootComponent(LightRoot);
			LightRoot->RegisterComponent();
			LightRoot->SetWorldLocation(LightPos);

			UPointLightComponent* Light = NewObject<UPointLightComponent>(LightActor);
			Light->SetupAttachment(LightRoot);
			Light->RegisterComponent();
			Light->SetWorldLocation(LightPos);

			// Classic fluorescent yellow-white light
			Light->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
			Light->SetIntensity(8000.0f);
			Light->SetAttenuationRadius(800.0f);
			Light->SetCastShadows(false); // Better performance

			// Add flickering to some lights for horror atmosphere
			if (FMath::FRand() < 0.35f)
			{
				ULSFlickeringLightComponent* Flicker = NewObject<ULSFlickeringLightComponent>(LightActor);
				Flicker->RegisterComponent();

				// Random flicker pattern
				float PatternRoll = FMath::FRand();
				if (PatternRoll < 0.4f)
				{
					Flicker->SetFlickerPattern(ELSFlickerPattern::Fluorescent);
				}
				else if (PatternRoll < 0.7f)
				{
					Flicker->SetFlickerPattern(ELSFlickerPattern::Dropout);
				}
				else
				{
					Flicker->SetFlickerPattern(ELSFlickerPattern::Horror);
				}
			}
		}
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("Spawned lighting"));
}

void ALSBackroomsGenerator::SpawnPlayerStart()
{
	UWorld* World = GetWorld();
	if (!World) return;

	PlayerSpawnLocation = FVector(2 * CellSize, 2 * CellSize, 50.0f);

	// Move existing PlayerStart to maze entrance instead of creating a new one
	TArray<AActor*> ExistingStarts;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), ExistingStarts);

	if (ExistingStarts.Num() > 0)
	{
		ExistingStarts[0]->GetRootComponent()->SetMobility(EComponentMobility::Movable);
		ExistingStarts[0]->SetActorLocation(PlayerSpawnLocation);
		UE_LOG(LogLiminalSpaces, Log, TEXT("Moved PlayerStart to %s"), *PlayerSpawnLocation.ToString());
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), PlayerSpawnLocation, FRotator::ZeroRotator, SpawnParams);
		UE_LOG(LogLiminalSpaces, Log, TEXT("Spawned PlayerStart at %s"), *PlayerSpawnLocation.ToString());
	}
}

void ALSBackroomsGenerator::SpawnEscapePoint()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALSEscapePoint* Escape = World->SpawnActor<ALSEscapePoint>(ALSEscapePoint::StaticClass(), EscapeLocation, FRotator::ZeroRotator, SpawnParams);
	if (Escape)
	{
		Escape->ActivateEscape();
		UE_LOG(LogLiminalSpaces, Log, TEXT("Spawned EscapePoint at %s"), *EscapeLocation.ToString());
	}
}

TArray<FIntPoint> ALSBackroomsGenerator::ComputePathToEscape() const
{
	// BFS from start (2,2) to escape grid coordinate, returns the cells along the shortest path.
	const FIntPoint Start(2, 2);
	const FIntPoint End(FMath::RoundToInt(EscapeLocation.X / CellSize),
		FMath::RoundToInt(EscapeLocation.Y / CellSize));

	TArray<FIntPoint> ResultPath;
	if (!IsPassage(Start.X, Start.Y) || !IsPassage(End.X, End.Y))
	{
		return ResultPath;
	}

	TArray<TArray<FIntPoint>> Parent;
	Parent.SetNum(GridSizeX);
	for (int32 X = 0; X < GridSizeX; X++)
	{
		Parent[X].Init(FIntPoint(-1, -1), GridSizeY);
	}

	TArray<FIntPoint> Queue;
	Queue.Add(Start);
	Parent[Start.X][Start.Y] = Start;

	const FIntPoint Dirs[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
	int32 Head = 0;
	bool bFound = false;
	while (Head < Queue.Num())
	{
		const FIntPoint Cur = Queue[Head++];
		if (Cur == End)
		{
			bFound = true;
			break;
		}
		for (const FIntPoint& D : Dirs)
		{
			const FIntPoint Next(Cur.X + D.X, Cur.Y + D.Y);
			if (Next.X < 0 || Next.X >= GridSizeX || Next.Y < 0 || Next.Y >= GridSizeY)
				continue;
			if (!IsPassage(Next.X, Next.Y))
				continue;
			if (Parent[Next.X][Next.Y].X != -1)
				continue;
			Parent[Next.X][Next.Y] = Cur;
			Queue.Add(Next);
		}
	}

	if (!bFound)
	{
		return ResultPath;
	}

	FIntPoint Cur = End;
	while (Cur != Start)
	{
		ResultPath.Insert(Cur, 0);
		Cur = Parent[Cur.X][Cur.Y];
	}
	ResultPath.Insert(Start, 0);
	return ResultPath;
}

void ALSBackroomsGenerator::SpawnNarrativeContent()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const TArray<FIntPoint> Path = ComputePathToEscape();
	if (Path.Num() < 5)
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("Path to escape too short to seed narrative content"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// === Clue notes ===
	// Place each clue along the path at evenly spaced fractions of distance to the exit,
	// but offset slightly into adjacent passages so they're discoveries, not road bumps.
	const TArray<LSStoryContent::FClueNote>& Clues = LSStoryContent::GetAllClues();
	const int32 NumClues = Clues.Num();
	for (int32 i = 0; i < NumClues; i++)
	{
		const float Frac = static_cast<float>(i + 1) / static_cast<float>(NumClues + 1);
		const int32 PathIdx = FMath::Clamp(FMath::RoundToInt(Frac * (Path.Num() - 1)), 1, Path.Num() - 1);
		const FIntPoint Cell = Path[PathIdx];

		// Try to nudge the clue into a side branch off the path so it feels less linear
		FIntPoint Spot = Cell;
		const FIntPoint Branches[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
		for (int32 b = 0; b < 4; b++)
		{
			const FIntPoint Candidate(Cell.X + Branches[b].X, Cell.Y + Branches[b].Y);
			if (IsPassage(Candidate.X, Candidate.Y) && !Path.Contains(Candidate))
			{
				Spot = Candidate;
				break;
			}
		}

		const FVector Loc(Spot.X * CellSize, Spot.Y * CellSize, 80.0f);
		ALSClueNote* Note = World->SpawnActor<ALSClueNote>(ALSClueNote::StaticClass(), Loc, FRotator::ZeroRotator, SpawnParams);
		if (Note)
		{
			Note->SetClueID(Clues[i].ClueID);
			UE_LOG(LogLiminalSpaces, Log, TEXT("Placed clue %s at %s"),
				*Clues[i].ClueID.ToString(), *Loc.ToString());
		}
	}

	// === Weapon pickup ===
	// Place the pipe roughly 30% of the way to the exit, on the critical path
	{
		const int32 PathIdx = FMath::Clamp(FMath::RoundToInt(Path.Num() * 0.30f), 1, Path.Num() - 2);
		const FIntPoint Cell = Path[PathIdx];
		const FVector Loc(Cell.X * CellSize, Cell.Y * CellSize, 60.0f);
		ALSWeaponPickup* Pickup = World->SpawnActor<ALSWeaponPickup>(ALSWeaponPickup::StaticClass(), Loc, FRotator::ZeroRotator, SpawnParams);
		if (Pickup)
		{
			Pickup->SetWeaponClass(ALSMeleeWeapon::StaticClass());
			Pickup->SetPickupLineID(FName("weapon_pickup"));
			UE_LOG(LogLiminalSpaces, Log, TEXT("Placed weapon pickup at %s"), *Loc.ToString());
		}
	}

	// === Atmospheric narrative triggers ===
	// Two triggers along the path: one calm/awe early, one curious mid-late
	const FName TriggerLines[] = { FName("calm_2"), FName("calm_4"), FName("curious_3") };
	const float TriggerFracs[] = { 0.15f, 0.45f, 0.70f };
	for (int32 i = 0; i < UE_ARRAY_COUNT(TriggerLines); i++)
	{
		const int32 PathIdx = FMath::Clamp(FMath::RoundToInt(TriggerFracs[i] * (Path.Num() - 1)), 1, Path.Num() - 1);
		const FIntPoint Cell = Path[PathIdx];
		const FVector Loc(Cell.X * CellSize, Cell.Y * CellSize, 100.0f);
		ALSNarrativeTrigger* Trigger = World->SpawnActor<ALSNarrativeTrigger>(ALSNarrativeTrigger::StaticClass(), Loc, FRotator::ZeroRotator, SpawnParams);
		if (Trigger)
		{
			Trigger->SetLineID(TriggerLines[i]);
			Trigger->SetExtents(FVector(CellSize * 0.45f, CellSize * 0.45f, 200.0f));
		}
	}
}
