#include "Combat/LSWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"
#include "LiminalSpaces.h"

ALSWeaponBase::ALSWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void ALSWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
}

void ALSWeaponBase::SetOwningCharacter(ACharacter* InCharacter)
{
	OwningCharacter = InCharacter;

	if (OwningCharacter)
	{
		AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon_r"));
	}
}

bool ALSWeaponBase::CanFire() const
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	return CurrentAmmo > 0 && (CurrentTime - LastFireTime) >= FireRate;
}

void ALSWeaponBase::StartFire()
{
	bWantsToFire = true;

	if (CanFire())
	{
		Fire();
	}

	if (bIsAutomatic)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALSWeaponBase::HandleAutoFire, FireRate, true);
	}
}

void ALSWeaponBase::StopFire()
{
	bWantsToFire = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ALSWeaponBase::Fire()
{
	if (!CanFire() || !OwningCharacter)
	{
		return;
	}

	CurrentAmmo--;
	LastFireTime = GetWorld()->GetTimeSeconds();

	// Hitscan trace from camera
	APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * Range);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(OwningCharacter);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, QueryParams);

	if (bHit && HitResult.GetActor())
	{
		FPointDamageEvent DamageEvent(Damage, HitResult, CameraRotation.Vector(), nullptr);
		HitResult.GetActor()->TakeDamage(Damage, DamageEvent, OwningCharacter->GetController(), this);
	}

	PlayFireEffects();
}

void ALSWeaponBase::PlayFireEffects()
{
	if (MuzzleFlashFX && WeaponMesh)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashFX,
			WeaponMesh,
			MuzzleSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (FireMontage && OwningCharacter)
	{
		if (UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(FireMontage);
		}
	}
}

void ALSWeaponBase::Reload()
{
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogLiminalSpaces, Log, TEXT("Weapon reloaded: %d/%d"), CurrentAmmo, MaxAmmo);
}

void ALSWeaponBase::HandleAutoFire()
{
	if (bWantsToFire && CanFire())
	{
		Fire();
	}
	else if (!bWantsToFire || CurrentAmmo <= 0)
	{
		StopFire();
	}
}
