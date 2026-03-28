#include "UI/LSMapSelectWidget.h"
#include "Data/LSMapDefinition.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Core/LSPlayerController.h"
#include "LiminalSpaces.h"

void ULSMapSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ULSMapSelectWidget::StartSelectedMap);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &ULSMapSelectWidget::GoBack);
	}

	PopulateMapList();
}

void ULSMapSelectWidget::PopulateMapList()
{
	// Map list population done in Blueprint using UMG
	// This provides the data, Blueprint handles the visual layout
}

void ULSMapSelectWidget::SelectMap(ULSMapDefinition* MapDefinition)
{
	SelectedMap = MapDefinition;

	if (SelectedMap)
	{
		UE_LOG(LogLiminalSpaces, Log, TEXT("Selected map: %s"), *SelectedMap->MapName);
	}
}

void ULSMapSelectWidget::StartSelectedMap()
{
	if (!SelectedMap)
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("No map selected"));
		return;
	}

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetOwningPlayer()))
	{
		PC->TravelToMap(SelectedMap->LevelPath.ToString());
	}
}

void ULSMapSelectWidget::GoBack()
{
	RemoveFromParent();
}
