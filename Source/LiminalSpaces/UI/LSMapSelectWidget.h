#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSMapSelectWidget.generated.h"

class ULSMapDefinition;
class UScrollBox;
class UButton;

UCLASS()
class LIMINALSPACES_API ULSMapSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|MapSelect")
	void PopulateMapList();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|MapSelect")
	void SelectMap(ULSMapDefinition* MapDefinition);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|MapSelect")
	void StartSelectedMap();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|MapSelect")
	void GoBack();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> MapListScrollBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LiminalSpaces|MapSelect")
	TArray<TObjectPtr<ULSMapDefinition>> AvailableMaps;

private:
	UPROPERTY()
	TObjectPtr<ULSMapDefinition> SelectedMap;
};
