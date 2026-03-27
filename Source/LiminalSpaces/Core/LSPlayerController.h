#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ALSHUD;

UCLASS()
class LIMINALSPACES_API ALSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALSPlayerController();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Navigation")
	void TravelToMap(const FString& MapPath);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Navigation")
	void ReturnToMainMenu();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputMappingContext> GameplayMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|UI")
	bool bIsPaused = false;

private:
	void HandlePauseInput();
};
