#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/LSTypes.h"
#include "LSMapDefinition.generated.h"

UCLASS(BlueprintType)
class LIMINALSPACES_API ULSMapDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	FText MapDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	FSoftObjectPath LevelPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	TObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	ELSMapDifficulty Difficulty = ELSMapDifficulty::Medium;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	int32 EstimatedMinutes = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	int32 MaxEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LiminalSpaces|Map")
	FString MapID;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("LSMap"), GetFName());
	}
};
