#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Narrative/LSNarrativeTypes.h"
#include "LSDialogWidget.generated.h"

class UTextBlock;
class UBorder;
class UVerticalBox;
class UCanvasPanel;

/**
 * Self-constructing dialog widget that shows the protagonist's monologue at the
 * bottom of the screen with a typewriter effect. Builds its own UMG tree in code
 * so it works with no Blueprint asset required.
 */
UCLASS()
class LIMINALSPACES_API ULSDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULSDialogWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MovedGeometry, float InDeltaTime) override;

	UFUNCTION()
	void HandleDialogLine(const FLSDialogLine& Line);

	UFUNCTION()
	void HandleNoteRead(FText Title, FText Body);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Dialog")
	void DismissNote();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Dialog")
	bool IsNoteOpen() const { return bIsNoteOpen; }

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Dialog")
	void ShowClueProgress(int32 Collected, int32 Total);

protected:
	// === Monologue (auto-built) ===
	UPROPERTY()
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY()
	TObjectPtr<UBorder> DialogBorder;

	UPROPERTY()
	TObjectPtr<UTextBlock> DialogText;

	// === Note overlay (auto-built) ===
	UPROPERTY()
	TObjectPtr<UBorder> NoteBackdrop;

	UPROPERTY()
	TObjectPtr<UTextBlock> NoteTitle;

	UPROPERTY()
	TObjectPtr<UTextBlock> NoteBody;

	UPROPERTY()
	TObjectPtr<UTextBlock> NoteHint;

	// === Clue counter ===
	UPROPERTY()
	TObjectPtr<UTextBlock> ClueCounterText;

	void BuildWidgetTree();
	void TickTypewriter(float DeltaTime);

	FString FullText;
	FString DisplayedText;
	float CharsPerSecond = 45.0f;
	float TypeAccumulator = 0.0f;
	float HoldTimeRemaining = 0.0f;
	float FadeAlpha = 0.0f;
	bool bIsTyping = false;
	bool bIsHolding = false;
	bool bIsFadingOut = false;
	bool bIsNoteOpen = false;
	float NoteAutoDismissAt = 0.0f;
	ELSDialogMood CurrentMood = ELSDialogMood::Calm;
};
