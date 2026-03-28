#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class LIMINALSPACES_API ULSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void UpdateHealth(float HealthPercent);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowInteractionPrompt(const FText& Prompt);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void HideInteractionPrompt();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowEscapeMessage(float CompletionTime);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|HUD")
	void ShowDamageIndicator();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InteractionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CrosshairImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EscapeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> DamageOverlay;
};
