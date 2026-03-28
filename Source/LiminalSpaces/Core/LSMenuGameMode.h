#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LSMenuGameMode.generated.h"

class ULSMainMenuWidget;

UCLASS()
class LIMINALSPACES_API ALSMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALSMenuGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Menu")
	TSubclassOf<ULSMainMenuWidget> MainMenuWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<ULSMainMenuWidget> MainMenuWidget;
};
