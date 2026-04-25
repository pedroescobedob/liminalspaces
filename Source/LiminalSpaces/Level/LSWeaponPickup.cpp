#include "Level/LSWeaponPickup.h"
#include "Combat/LSWeaponBase.h"
#include "Combat/LSCombatComponent.h"
#include "Combat/LSMeleeWeapon.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "Player/LSCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"

ALSWeaponPickup::ALSWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(SceneRoot);
	PickupMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		PickupMesh->SetStaticMesh(CylinderMesh.Object);
		PickupMesh->SetRelativeScale3D(FVector(0.10f, 0.10f, 0.85f));
		PickupMesh->SetRelativeRotation(FRotator(60.0f, 0.0f, 0.0f));
	}

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(SceneRoot);
	Glow->SetIntensity(1500.0f);
	Glow->SetLightColor(FLinearColor(0.4f, 0.7f, 1.0f));
	Glow->SetAttenuationRadius(450.0f);
	Glow->SetCastShadows(false);
	Glow->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(SceneRoot);
	InteractionVolume->SetSphereRadius(140.0f);
	InteractionVolume->SetCollisionProfileName(TEXT("Interactable"));

	WeaponClass = ALSMeleeWeapon::StaticClass();
}

void ALSWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	if (PickupMesh)
	{
		InitialMeshLocation = PickupMesh->GetRelativeLocation();
		if (UMaterialInterface* BaseMat = PickupMesh->GetMaterial(0))
		{
			UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (MID)
			{
				MID->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.55f, 0.55f, 0.60f));
				PickupMesh->SetMaterial(0, MID);
			}
		}
	}
}

void ALSWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bConsumed || !PickupMesh)
	{
		return;
	}

	TimeAccumulator += DeltaTime;
	const float Bob = FMath::Sin(TimeAccumulator * 1.6f) * 8.0f;
	PickupMesh->SetRelativeLocation(InitialMeshLocation + FVector(0.0f, 0.0f, Bob));
	PickupMesh->AddRelativeRotation(FRotator(0.0f, DeltaTime * 25.0f, 0.0f));
}

bool ALSWeaponPickup::CanInteract_Implementation(ACharacter* InteractingCharacter) const
{
	return !bConsumed && WeaponClass != nullptr;
}

void ALSWeaponPickup::Interact_Implementation(ACharacter* InteractingCharacter)
{
	if (bConsumed || !WeaponClass)
	{
		return;
	}

	ALSCharacter* PlayerChar = Cast<ALSCharacter>(InteractingCharacter);
	if (!PlayerChar)
	{
		return;
	}

	if (ULSCombatComponent* Combat = PlayerChar->GetCombatComponent())
	{
		Combat->EquipWeapon(WeaponClass);
	}

	if (PickupLineID != NAME_None)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
			{
				Narr->RequestLine(PickupLineID);
			}
		}
	}

	bConsumed = true;
	if (PickupMesh)
	{
		PickupMesh->SetVisibility(false);
		PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (Glow)
	{
		Glow->SetVisibility(false);
	}
	SetLifeSpan(0.5f);
}

FText ALSWeaponPickup::GetInteractionPrompt_Implementation() const
{
	return PickupPromptText;
}
