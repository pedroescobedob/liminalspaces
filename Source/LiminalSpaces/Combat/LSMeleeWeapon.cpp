#include "Combat/LSMeleeWeapon.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

ALSMeleeWeapon::ALSMeleeWeapon()
{
	WeaponType = ELSWeaponType::Melee;
	Damage = 50.0f;
	FireRate = 0.55f;
	Range = 220.0f;
	MaxAmmo = 9999;     // Effectively infinite for melee
	bIsAutomatic = false;
}

void ALSMeleeWeapon::Fire()
{
	if (!CanFire() || !OwningCharacter)
	{
		return;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();

	APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector Start = OwningCharacter->GetActorLocation();
	const FVector End = Start + (CameraRotation.Vector() * SwingRange);

	TArray<FHitResult> Hits;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(OwningCharacter);

	GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SwingRadius),
		QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == OwningCharacter)
		{
			continue;
		}
		FPointDamageEvent DamageEvent(Damage, Hit, CameraRotation.Vector(), nullptr);
		HitActor->TakeDamage(Damage, DamageEvent, OwningCharacter->GetController(), this);
	}

	PlayFireEffects();

	UE_LOG(LogLiminalSpaces, Verbose, TEXT("Melee swing hit %d actors"), Hits.Num());
}
