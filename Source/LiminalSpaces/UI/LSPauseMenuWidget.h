#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSPauseMenuWidget.generated.h"

class UButton;

UCLASS()
class LIMINALSPACES_API ULSPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnResumeClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnSettingsClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnMainMenuClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnQuitClicked();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> MainMenuButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;
};
