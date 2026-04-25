#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSWinScreenWidget.generated.h"

class UTextBlock;
class UBorder;
class UVerticalBox;
class UCanvasPanel;

/**
 * Final win screen — fades in over the gameplay viewport with completion stats and credits.
 * Self-builds its UMG tree so it works without a Blueprint asset.
 */
UCLASS()
class LIMINALSPACES_API ULSWinScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULSWinScreenWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MovedGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void ShowResults(float CompletionTime, int32 CluesCollected, int32 CluesTotal);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void ReturnToMenu();

protected:
	UPROPERTY()
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY()
	TObjectPtr<UBorder> Backdrop;

	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SubtitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> StatsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> CreditsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> HintText;

	void BuildWidgetTree();

	float FadeAlpha = 0.0f;
};
