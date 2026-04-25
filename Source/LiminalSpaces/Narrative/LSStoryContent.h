#pragma once

#include "CoreMinimal.h"
#include "Narrative/LSNarrativeTypes.h"

/**
 * Static catalog of all dialog lines and clue notes for Liminal Spaces.
 * Kept as a header-only static table so it ships without depending on UAsset content.
 */
namespace LSStoryContent
{
	struct FClueNote
	{
		FName ClueID;
		FText Title;
		FText Body;
		// Line that fires automatically after the player reads the note
		FName FollowupLineID;
	};

	// === MONOLOGUE LINES ===
	// The protagonist wakes with no memory. Lines guide the player through:
	// 1. Confusion (intro)
	// 2. Curiosity / awe at the spaces
	// 3. Discovery via clues
	// 4. Tension / danger when enemies appear
	// 5. Resolution at the escape

	inline const TArray<FLSDialogLine>& GetAllLines()
	{
		static const TArray<FLSDialogLine> Lines = {
			// --- INTRO ---
			{ FName("intro_1"), FText::FromString(TEXT("...where am I?")),
				ELSDialogMood::Confused, ELSDialogPriority::Important, 4.0f, true },
			{ FName("intro_2"), FText::FromString(TEXT("My head is pounding. I can't remember how I got here.")),
				ELSDialogMood::Confused, ELSDialogPriority::Important, 5.0f, true },
			{ FName("intro_3"), FText::FromString(TEXT("These walls... I've never seen this place before. Have I?")),
				ELSDialogMood::Confused, ELSDialogPriority::Normal, 5.0f, true },
			{ FName("intro_4"), FText::FromString(TEXT("I should keep moving. There has to be a way out.")),
				ELSDialogMood::Curious, ELSDialogPriority::Normal, 4.5f, true },

			// --- CALM / AWE ---
			{ FName("calm_1"), FText::FromString(TEXT("It's strange... the silence here is almost peaceful.")),
				ELSDialogMood::Calm, ELSDialogPriority::Ambient, 4.5f, true },
			{ FName("calm_2"), FText::FromString(TEXT("The light hums. The walls breathe. There's a kind of beauty to it.")),
				ELSDialogMood::Awe, ELSDialogPriority::Ambient, 5.0f, true },
			{ FName("calm_3"), FText::FromString(TEXT("If I weren't so afraid, I could almost rest here.")),
				ELSDialogMood::Calm, ELSDialogPriority::Ambient, 5.0f, true },
			{ FName("calm_4"), FText::FromString(TEXT("Empty rooms. Endless corridors. It's like the world forgot itself.")),
				ELSDialogMood::Awe, ELSDialogPriority::Ambient, 5.5f, true },
			{ FName("calm_5"), FText::FromString(TEXT("The carpet is so soft under my feet. When was the last time I noticed something like that?")),
				ELSDialogMood::Calm, ELSDialogPriority::Ambient, 5.5f, true },
			{ FName("calm_6"), FText::FromString(TEXT("This place... it shouldn't feel familiar. But part of me knows it.")),
				ELSDialogMood::Curious, ELSDialogPriority::Normal, 5.0f, true },

			// --- CURIOSITY ---
			{ FName("curious_1"), FText::FromString(TEXT("Did I leave something behind? Why does that matter so much right now?")),
				ELSDialogMood::Curious, ELSDialogPriority::Ambient, 5.0f, true },
			{ FName("curious_2"), FText::FromString(TEXT("Footprints in the dust. Mine? Or someone else's?")),
				ELSDialogMood::Curious, ELSDialogPriority::Normal, 4.5f, true },
			{ FName("curious_3"), FText::FromString(TEXT("Wait — I think I've been here before. Just not... like this.")),
				ELSDialogMood::Curious, ELSDialogPriority::Normal, 5.0f, true },

			// --- TENSE ---
			{ FName("tense_1"), FText::FromString(TEXT("Something's not right. The air just changed.")),
				ELSDialogMood::Tense, ELSDialogPriority::Critical, 4.0f, false },
			{ FName("tense_2"), FText::FromString(TEXT("I'm not alone in here.")),
				ELSDialogMood::Tense, ELSDialogPriority::Critical, 3.5f, false },
			{ FName("tense_3"), FText::FromString(TEXT("Did you hear that? Tell me you heard that.")),
				ELSDialogMood::Tense, ELSDialogPriority::Critical, 4.0f, false },

			// --- DANGER ---
			{ FName("danger_1"), FText::FromString(TEXT("RUN — go, GO!")),
				ELSDialogMood::Danger, ELSDialogPriority::Critical, 3.0f, false },
			{ FName("danger_2"), FText::FromString(TEXT("I have to fight. I have to.")),
				ELSDialogMood::Danger, ELSDialogPriority::Critical, 3.5f, false },
			{ FName("danger_3"), FText::FromString(TEXT("Don't look back — keep moving!")),
				ELSDialogMood::Danger, ELSDialogPriority::Critical, 3.0f, false },

			// --- WEAPON PICKUP ---
			{ FName("weapon_pickup"), FText::FromString(TEXT("A pipe. Heavy. It'll do.")),
				ELSDialogMood::Discovery, ELSDialogPriority::Important, 4.0f, true },

			// --- POST-CLUE FOLLOWUPS ---
			{ FName("after_clue_1"), FText::FromString(TEXT("My handwriting. So this is mine. But I don't remember writing it.")),
				ELSDialogMood::Discovery, ELSDialogPriority::Important, 5.5f, true },
			{ FName("after_clue_2"), FText::FromString(TEXT("Anna. The name brings tears I can't explain.")),
				ELSDialogMood::Confused, ELSDialogPriority::Important, 5.0f, true },
			{ FName("after_clue_3"), FText::FromString(TEXT("The accident. I'm starting to remember the accident.")),
				ELSDialogMood::Discovery, ELSDialogPriority::Important, 5.5f, true },
			{ FName("after_clue_4"), FText::FromString(TEXT("I wasn't supposed to wake up. So why am I here?")),
				ELSDialogMood::Confused, ELSDialogPriority::Important, 5.5f, true },
			{ FName("after_clue_5"), FText::FromString(TEXT("The way out... it's where I always knew it would be.")),
				ELSDialogMood::Resolution, ELSDialogPriority::Important, 5.0f, true },

			// --- ESCAPE / RESOLUTION ---
			{ FName("escape_locked"), FText::FromString(TEXT("It won't open. Not yet. Something's still missing.")),
				ELSDialogMood::Tense, ELSDialogPriority::Important, 4.5f, false },
			{ FName("escape_almost"), FText::FromString(TEXT("I can feel it now. The door is real. I just need to remember a little more.")),
				ELSDialogMood::Discovery, ELSDialogPriority::Important, 5.5f, true },
			{ FName("escape_open"), FText::FromString(TEXT("I'm ready. Whatever's on the other side... I'm ready.")),
				ELSDialogMood::Resolution, ELSDialogPriority::Critical, 5.0f, true },
		};
		return Lines;
	}

	// === CLUE NOTES ===
	// Five collectible notes that gate the escape.
	inline const TArray<FClueNote>& GetAllClues()
	{
		static const TArray<FClueNote> Clues = {
			{
				FName("clue_1_journal"),
				FText::FromString(TEXT("A torn journal page")),
				FText::FromString(TEXT("Day 3.\n\nI can't tell if this is real anymore. Same hallways. Same yellow walls. I keep finding pieces of myself scribbled on paper, like someone left them for me to find.\n\nMaybe I left them.\n\nThere are doors that weren't there yesterday. There are corridors I'm sure I walked through twice that lead to different rooms each time.\n\nIf you find this — if YOU is even me — please. Keep going. The way out is real.")),
				FName("after_clue_1")
			},
			{
				FName("clue_2_photo"),
				FText::FromString(TEXT("A faded photograph")),
				FText::FromString(TEXT("Two people on a porch in summer. The colors are bleeding away at the edges, like the photo is slowly becoming part of this place.\n\nOn the back, in pen:\n\n\"Anna and me — the day before everything.\"\n\nThe woman in the photo is smiling. I'm smiling next to her. I don't remember her face, but my chest aches looking at it.")),
				FName("after_clue_2")
			},
			{
				FName("clue_3_hospital"),
				FText::FromString(TEXT("A hospital wristband")),
				FText::FromString(TEXT("Plastic. Faded ink. Barely legible:\n\nNAME: [redacted]\nDOB: 04/12/1987\nADMITTED: 03:47\nCONDITION: CRITICAL\n\nThere's something else under it — another wristband, beneath my skin almost. As if I'd been wearing it so long it became part of me.\n\nThere was an accident. There was a road. There was rain.")),
				FName("after_clue_3")
			},
			{
				FName("clue_4_letter"),
				FText::FromString(TEXT("An unsent letter")),
				FText::FromString(TEXT("\"My love,\n\nI know you can hear me, even if the doctors say you can't. I sit here every day and I tell you about the garden, about the dog, about all the small ordinary things you used to laugh at me for noticing.\n\nThe machines say your brain is still in there. Somewhere. Walking. Looking for a way back.\n\nI'll keep waiting.\n\n— A.\"")),
				FName("after_clue_4")
			},
			{
				FName("clue_5_door"),
				FText::FromString(TEXT("A child's drawing")),
				FText::FromString(TEXT("A crayon drawing. A door at the end of a long yellow hallway. A small figure walking toward it. Above the door, in shaky letters:\n\n\"WAKE UP\"\n\nI know this drawing. I made this drawing. When I was small, I would dream of being lost in long hallways, and my mother would tell me: when you find the door, you'll know it's time.\n\nI'm so close now. I can feel her hand on mine.")),
				FName("after_clue_5")
			},
		};
		return Clues;
	}

	inline const FLSDialogLine* FindLine(FName LineID)
	{
		for (const FLSDialogLine& Line : GetAllLines())
		{
			if (Line.LineID == LineID)
			{
				return &Line;
			}
		}
		return nullptr;
	}

	inline const FClueNote* FindClue(FName ClueID)
	{
		for (const FClueNote& Clue : GetAllClues())
		{
			if (Clue.ClueID == ClueID)
			{
				return &Clue;
			}
		}
		return nullptr;
	}

	inline int32 GetTotalClueCount()
	{
		return GetAllClues().Num();
	}
}
