#pragma once

#include "CoreMinimal.h"
#include "LSNarrativeTypes.generated.h"

UENUM(BlueprintType)
enum class ELSDialogMood : uint8
{
	Calm,        // Reflective, accepting
	Awe,         // Beauty appreciation
	Curious,     // Investigating
	Confused,    // Memory fragments
	Tense,       // Enemy nearby but not attacking
	Danger,      // Active enemy chase
	Discovery,   // Found a clue
	Resolution   // Ending lines
};

UENUM(BlueprintType)
enum class ELSDialogPriority : uint8
{
	Ambient = 0,    // Background flavor; gets dropped if higher priority comes in
	Normal = 1,     // Standard story line
	Important = 2,  // Clue/discovery lines
	Critical = 3    // Danger/death lines, always interrupt
};

USTRUCT(BlueprintType)
struct FLSDialogLine
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName LineID;

	UPROPERTY(BlueprintReadWrite)
	FText Text;

	UPROPERTY(BlueprintReadWrite)
	ELSDialogMood Mood = ELSDialogMood::Calm;

	UPROPERTY(BlueprintReadWrite)
	ELSDialogPriority Priority = ELSDialogPriority::Normal;

	// How long the line stays on screen after the typewriter finishes
	UPROPERTY(BlueprintReadWrite)
	float HoldDuration = 4.5f;

	// If true, will only ever play once per game session
	UPROPERTY(BlueprintReadWrite)
	bool bOneShot = true;
};
