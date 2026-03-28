#include "Level/LSBackroomsGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Level/LSEscapePoint.h"
#include "Level/LSDoorActor.h"
#include "Level/LSFlickeringLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ALSBackroomsGenerator::ALSBackroomsGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create instanced mesh components
	FloorMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorMeshes"));
	FloorMeshes->SetupAttachment(RootComponent);
	FloorMeshes->SetMobility(EComponentMobility::Static);

	CeilingMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CeilingMeshes"));
	CeilingMeshes->SetupAttachment(RootComponent);
	CeilingMeshes->SetMobility(EComponentMobility::Static);

	WallMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallMeshes"));
	WallMeshes->SetupAttachment(RootComponent);
	WallMeshes->SetMobility(EComponentMobility::Static);

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
	GenerateLevel();
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
}

void ALSBackroomsGenerator::CreateMaterials()
{
	UMaterial* BaseMat = UMaterial::GetDefaultMaterial(MD_Surface);

	// Yellowish wallpaper walls
	WallMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	WallMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.82f, 0.72f, 0.42f));
	WallMeshes->SetMaterial(0, WallMaterial);

	// Dark carpet floor
	FloorMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	FloorMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.35f, 0.30f, 0.22f));
	FloorMeshes->SetMaterial(0, FloorMaterial);

	// Off-white ceiling
	CeilingMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	CeilingMaterial->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.88f, 0.85f, 0.78f));
	CeilingMeshes->SetMaterial(0, CeilingMaterial);
}

void ALSBackroomsGenerator::GenerateMaze()
{
	// Initialize grid - all cells are walls
	Grid.SetNum(GridSizeX);
	for (int32 X = 0; X < GridSizeX; X++)
	{
		Grid[X].SetNum(GridSizeY);
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			Grid[X][Y] = false;
		}
	}

	// Use DFS maze generation (recursive backtracker)
	TArray<TArray<bool>> Visited;
	Visited.SetNum(GridSizeX);
	for (int32 X = 0; X < GridSizeX; X++)
	{
		Visited[X].SetNum(GridSizeY);
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			Visited[X][Y] = false;
		}
	}

	// Start from (1,1) to ensure border walls
	Grid[1][1] = true;
	CarveMaze(1, 1, Visited);

	// Create some extra openings for a more open feel (less maze-like, more backrooms)
	for (int32 X = 1; X < GridSizeX - 1; X++)
	{
		for (int32 Y = 1; Y < GridSizeY - 1; Y++)
		{
			if (!Grid[X][Y] && FMath::FRand() < 0.15f)
			{
				// Check if adjacent to at least 2 passage cells
				int32 AdjacentPassages = 0;
				if (X > 0 && Grid[X - 1][Y]) AdjacentPassages++;
				if (X < GridSizeX - 1 && Grid[X + 1][Y]) AdjacentPassages++;
				if (Y > 0 && Grid[X][Y - 1]) AdjacentPassages++;
				if (Y < GridSizeY - 1 && Grid[X][Y + 1]) AdjacentPassages++;

				if (AdjacentPassages >= 2)
				{
					Grid[X][Y] = true;
				}
			}
		}
	}

	// Find the farthest passage from start for escape point
	int32 MaxDist = 0;
	EscapeLocation = FVector(1 * CellSize, 1 * CellSize, 50.0f);
	for (int32 X = 1; X < GridSizeX - 1; X++)
	{
		for (int32 Y = 1; Y < GridSizeY - 1; Y++)
		{
			if (Grid[X][Y])
			{
				int32 Dist = FMath::Abs(X - 1) + FMath::Abs(Y - 1);
				if (Dist > MaxDist)
				{
					MaxDist = Dist;
					EscapeLocation = FVector(X * CellSize, Y * CellSize, 50.0f);
				}
			}
		}
	}
}

void ALSBackroomsGenerator::CarveMaze(int32 X, int32 Y, TArray<TArray<bool>>& Visited)
{
	Visited[X][Y] = true;

	// Directions: N, S, E, W (step by 2 to leave wall cells between)
	struct FDir { int32 DX, DY; };
	TArray<FDir> Dirs = { {0, 2}, {0, -2}, {2, 0}, {-2, 0} };

	// Shuffle directions
	for (int32 i = Dirs.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Dirs.Swap(i, j);
	}

	for (const FDir& Dir : Dirs)
	{
		int32 NX = X + Dir.DX;
		int32 NY = Y + Dir.DY;

		if (NX >= 1 && NX < GridSizeX - 1 && NY >= 1 && NY < GridSizeY - 1 && !Visited[NX][NY])
		{
			// Carve passage between current and neighbor
			Grid[X + Dir.DX / 2][Y + Dir.DY / 2] = true;
			Grid[NX][NY] = true;
			CarveMaze(NX, NY, Visited);
		}
	}
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

	for (int32 X = 0; X < GridSizeX; X++)
	{
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			if (!Grid[X][Y])
				continue;

			// Place a light every 2-3 cells for that classic fluorescent grid look
			bool bPlaceLight = (X % 3 == 1 && Y % 2 == 1) || (X % 2 == 0 && Y % 3 == 1);
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
			Light->SetIntensity(3000.0f);
			Light->SetAttenuationRadius(600.0f);
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

	PlayerSpawnLocation = FVector(1 * CellSize, 1 * CellSize, 50.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APlayerStart* Start = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), PlayerSpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (Start)
	{
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
