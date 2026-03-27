#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSMainMenuWidget.generated.h"

class UMediaPlayer;
class UMediaSource;
class UButton;
class UImage;
class UTextBlock;

UCLASS()
class LIMINALSPACES_API ULSMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void StartVideoBackground();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void StopVideoBackground();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnPlayClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnSettingsClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnLoginClicked();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Menu")
	void OnQuitClicked();

	// Video background
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Menu|Video")
	TObjectPtr<UMediaPlayer> BackgroundMediaPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|Menu|Video")
	TObjectPtr<UMediaSource> BackgroundMediaSource;

	// Widget bindings (bound in UMG Blueprint)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> LoginButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> LogoImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> VideoBackgroundImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GameTitle;
};
