#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSEnemyHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class LIMINALSPACES_API ULSEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void SetHealthPercent(float Percent);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|UI")
	void SetEnemyName(const FText& Name);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnemyNameText;
};
