#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSBackroomsGenerator.generated.h"

class UInstancedStaticMeshComponent;
class UPointLightComponent;

UCLASS()
class LIMINALSPACES_API ALSBackroomsGenerator : public AActor
{
	GENERATED_BODY()

public:
	ALSBackroomsGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	int32 GridSizeX = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	int32 GridSizeY = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	float CellSize = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	float WallHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	float WallThickness = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiminalSpaces|Generation")
	int32 RandomSeed = 0;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Generation")
	void GenerateLevel();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Generation")
	FVector GetPlayerSpawnLocation() const { return PlayerSpawnLocation; }

protected:
	virtual void BeginPlay() override;

private:
	// Instanced mesh components for performance
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> FloorMeshes;

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> CeilingMeshes;

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> WallMeshes;

	// Grid data: true = passage, false = wall
	TArray<TArray<bool>> Grid;

	FVector PlayerSpawnLocation;
	FVector EscapeLocation;

	void GenerateMaze();
	void CarveMaze(int32 X, int32 Y, TArray<TArray<bool>>& Visited);
	void SpawnGeometry();
	void SpawnLighting();
	void SpawnPlayerStart();
	void SpawnEscapePoint();
	void SpawnDoors();

	bool IsPassage(int32 X, int32 Y) const;

	void CreateMaterials();
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> WallMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FloorMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CeilingMaterial;
};
