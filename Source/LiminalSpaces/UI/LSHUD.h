#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LSHUD.generated.h"

class ULSHUDWidget;
class ULSPauseMenuWidget;
class ULSDialogWidget;
class ULSWinScreenWidget;

UCLASS()
class LIMINALSPACES_API ALSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALSHUD();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|HUD")
	ULSHUDWidget* GetHUDWidget() const { return HUDWidget; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|HUD")
	ULSDialogWidget* GetDialogWidget() const { return DialogWidget; }

	virtual void ShowHUD() override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowEscapeScreen(float CompletionTime);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowWinScreen(float CompletionTime, int32 CluesCollected);

	UFUNCTION()
	void HandleClueCollected(FName ClueID);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSDialogWidget> DialogWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|HUD")
	TSubclassOf<ULSWinScreenWidget> WinScreenWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<ULSHUDWidget> HUDWidget;

	UPROPERTY()
	TObjectPtr<ULSPauseMenuWidget> PauseMenuWidget;

	UPROPERTY()
	TObjectPtr<ULSDialogWidget> DialogWidget;

	UPROPERTY()
	TObjectPtr<ULSWinScreenWidget> WinScreenWidget;
};
