#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LSHUD.generated.h"

class ULSHUDWidget;
class ULSPauseMenuWidget;

UCLASS()
class LIMINALSPACES_API ALSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALSHUD();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|HUD")
	ULSHUDWidget* GetHUDWidget() const { return HUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowEscapeScreen(float CompletionTime);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSPauseMenuWidget> PauseMenuWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<ULSHUDWidget> HUDWidget;

	UPROPERTY()
	TObjectPtr<ULSPauseMenuWidget> PauseMenuWidget;
};
