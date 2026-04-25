#include "Narrative/LSNarrativeSubsystem.h"
#include "Narrative/LSStoryContent.h"
#include "Engine/World.h"
#include "LiminalSpaces.h"

void ULSNarrativeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogLiminalSpaces, Log, TEXT("Narrative subsystem initialized"));
}

void ULSNarrativeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void ULSNarrativeSubsystem::RequestLine(FName LineID)
{
	const FLSDialogLine* Line = LSStoryContent::FindLine(LineID);
	if (!Line)
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("Requested unknown dialog line: %s"), *LineID.ToString());
		return;
	}

	if (Line->bOneShot && ShownLines.Contains(LineID))
	{
		return;
	}

	ShownLines.Add(LineID);
	OnDialogLineRequested.Broadcast(*Line);

	UE_LOG(LogLiminalSpaces, Log, TEXT("Dialog: [%s] %s"),
		*LineID.ToString(), *Line->Text.ToString());
}

void ULSNarrativeSubsystem::RequestRandomCalmLine()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastCalmLineTime < CalmLineCooldown)
	{
		return;
	}

	// In danger: prefer tense lines instead
	const ELSDialogMood TargetMood = IsInDanger() ? ELSDialogMood::Tense : ELSDialogMood::Calm;

	// Collect all unshown lines matching the target mood
	TArray<FName> Candidates;
	for (const FLSDialogLine& Line : LSStoryContent::GetAllLines())
	{
		if (Line.Mood != TargetMood && !(TargetMood == ELSDialogMood::Calm && Line.Mood == ELSDialogMood::Awe))
		{
			continue;
		}
		if (Line.bOneShot && ShownLines.Contains(Line.LineID))
		{
			continue;
		}
		Candidates.Add(Line.LineID);
	}

	if (Candidates.Num() == 0)
	{
		return;
	}

	const FName Pick = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	LastCalmLineTime = Now;
	RequestLine(Pick);
}

void ULSNarrativeSubsystem::NotifyEnemyAlerted()
{
	const bool bWasSafe = (ActiveThreatCount == 0);
	ActiveThreatCount++;

	if (bWasSafe)
	{
		// Pick a tense or danger line that hasn't been overused
		static const FName TenseLines[] = {
			FName("tense_1"), FName("tense_2"), FName("tense_3"),
			FName("danger_1"), FName("danger_2"), FName("danger_3")
		};
		const int32 Idx = FMath::RandRange(0, UE_ARRAY_COUNT(TenseLines) - 1);
		// Force these to be replayable by removing from shown set
		ShownLines.Remove(TenseLines[Idx]);
		RequestLine(TenseLines[Idx]);
	}
}

void ULSNarrativeSubsystem::NotifyEnemyLost()
{
	ActiveThreatCount = FMath::Max(0, ActiveThreatCount - 1);
}

bool ULSNarrativeSubsystem::CollectClue(FName ClueID)
{
	if (CollectedClues.Contains(ClueID))
	{
		return false;
	}

	const LSStoryContent::FClueNote* Clue = LSStoryContent::FindClue(ClueID);
	if (!Clue)
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("Tried to collect unknown clue: %s"), *ClueID.ToString());
		return false;
	}

	CollectedClues.Add(ClueID);
	OnClueCollected.Broadcast(ClueID);
	OnNoteRead.Broadcast(Clue->Title, Clue->Body);

	if (Clue->FollowupLineID != NAME_None)
	{
		RequestLine(Clue->FollowupLineID);
	}

	UE_LOG(LogLiminalSpaces, Log, TEXT("Clue collected: %s (%d/%d)"),
		*ClueID.ToString(), GetCluesCollectedCount(), GetTotalClueCount());

	return true;
}

int32 ULSNarrativeSubsystem::GetTotalClueCount() const
{
	return LSStoryContent::GetTotalClueCount();
}

bool ULSNarrativeSubsystem::IsEscapeUnlocked() const
{
	return GetCluesCollectedCount() >= CluesRequiredToEscape;
}

void ULSNarrativeSubsystem::ResetForNewGame()
{
	ShownLines.Empty();
	CollectedClues.Empty();
	ActiveThreatCount = 0;
	LastCalmLineTime = -100.0f;
}
