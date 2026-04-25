#include "Narrative/LSClueNote.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"

ALSClueNote::ALSClueNote()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	NoteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NoteMesh"));
	NoteMesh->SetupAttachment(SceneRoot);
	NoteMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// A flat thin "page" using the engine cube mesh, scaled to look like paper
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		NoteMesh->SetStaticMesh(CubeMesh.Object);
		NoteMesh->SetRelativeScale3D(FVector(0.18f, 0.24f, 0.01f));
	}

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(SceneRoot);
	Glow->SetIntensity(800.0f);
	Glow->SetLightColor(FLinearColor(1.0f, 0.85f, 0.55f));
	Glow->SetAttenuationRadius(350.0f);
	Glow->SetCastShadows(false);
	Glow->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(SceneRoot);
	InteractionVolume->SetSphereRadius(120.0f);
	InteractionVolume->SetCollisionProfileName(TEXT("Interactable"));
}

void ALSClueNote::BeginPlay()
{
	Super::BeginPlay();

	if (NoteMesh)
	{
		InitialMeshLocation = NoteMesh->GetRelativeLocation();

		// Tint it warm yellow so it reads as "paper"
		if (UMaterialInterface* BaseMat = NoteMesh->GetMaterial(0))
		{
			UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (MID)
			{
				MID->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.95f, 0.88f, 0.55f));
				NoteMesh->SetMaterial(0, MID);
			}
		}
	}
}

void ALSClueNote::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bConsumed || !NoteMesh)
	{
		return;
	}

	// Gentle hover + rotation to draw the player's attention
	TimeAccumulator += DeltaTime;
	const float Bob = FMath::Sin(TimeAccumulator * 2.2f) * 6.0f;
	NoteMesh->SetRelativeLocation(InitialMeshLocation + FVector(0.0f, 0.0f, Bob));
	NoteMesh->AddRelativeRotation(FRotator(0.0f, DeltaTime * 30.0f, 0.0f));
}

bool ALSClueNote::CanInteract_Implementation(ACharacter* InteractingCharacter) const
{
	return !bConsumed;
}

void ALSClueNote::Interact_Implementation(ACharacter* InteractingCharacter)
{
	if (bConsumed)
	{
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			if (Narr->CollectClue(ClueID))
			{
				bConsumed = true;
				if (Glow)
				{
					Glow->SetVisibility(false);
				}
				if (NoteMesh)
				{
					NoteMesh->SetVisibility(false);
					NoteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
				SetLifeSpan(0.5f);
			}
		}
	}
}

FText ALSClueNote::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(TEXT("[E] Read"));
}
