#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Narrative/LSNarrativeTypes.h"
#include "LSNarrativeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogLineRequested, const FLSDialogLine&, Line);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClueCollected, FName, ClueID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNoteRead, FText, Title, FText, Body);

/**
 * Coordinates the protagonist's monologue and the clue/note collection.
 * GameInstance-scoped so it persists across map transitions during a single playthrough.
 */
UCLASS()
class LIMINALSPACES_API ULSNarrativeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// === Dialog ===

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void RequestLine(FName LineID);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void RequestRandomCalmLine();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	bool HasShownLine(FName LineID) const { return ShownLines.Contains(LineID); }

	// Increases on enemy alert, decreases on enemy lost. Drives mood selection.
	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void NotifyEnemyAlerted();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void NotifyEnemyLost();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	bool IsInDanger() const { return ActiveThreatCount > 0; }

	// === Clues ===

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	bool CollectClue(FName ClueID);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	int32 GetCluesCollectedCount() const { return CollectedClues.Num(); }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	int32 GetTotalClueCount() const;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	bool HasCollectedClue(FName ClueID) const { return CollectedClues.Contains(ClueID); }

	// Returns true if all clues required to escape have been collected
	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	bool IsEscapeUnlocked() const;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Narrative")
	int32 GetCluesRequiredToEscape() const { return CluesRequiredToEscape; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Narrative")
	void ResetForNewGame();

	// === Delegates ===

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Narrative")
	FOnDialogLineRequested OnDialogLineRequested;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Narrative")
	FOnClueCollected OnClueCollected;

	UPROPERTY(BlueprintAssignable, Category = "LiminalSpaces|Narrative")
	FOnNoteRead OnNoteRead;

protected:
	// Set of line IDs already shown (one-shot lines won't fire again)
	UPROPERTY()
	TSet<FName> ShownLines;

	UPROPERTY()
	TSet<FName> CollectedClues;

	UPROPERTY()
	int32 ActiveThreatCount = 0;

	UPROPERTY()
	int32 CluesRequiredToEscape = 4;

	// Calm line cooldown so we don't spam the player while wandering
	UPROPERTY()
	float LastCalmLineTime = -100.0f;

	UPROPERTY()
	float CalmLineCooldown = 18.0f;
};
