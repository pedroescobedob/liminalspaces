#include "Player/LSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputActionValue.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Combat/LSHealthComponent.h"
#include "Combat/LSCombatComponent.h"
#include "Combat/LSMeleeWeapon.h"
#include "Level/LSInteractableBase.h"
#include "Player/LSInteractionComponent.h"
#include "Core/LSGameMode.h"
#include "Narrative/LSNarrativeSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "LiminalSpaces.h"

ALSCharacter::ALSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// First person arms mesh
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// Hide the third person mesh in first person
	GetMesh()->SetOwnerNoSee(true);

	// Components
	HealthComponent = CreateDefaultSubobject<ULSHealthComponent>(TEXT("HealthComponent"));
	CombatComponent = CreateDefaultSubobject<ULSCombatComponent>(TEXT("CombatComponent"));
	InteractionComponent = CreateDefaultSubobject<ULSInteractionComponent>(TEXT("InteractionComponent"));

	// Movement defaults
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->JumpZVelocity = 420.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Default starter weapon
	StarterUnarmedWeaponClass = ALSMeleeWeapon::StaticClass();

	// Bodycam VHS post-process — applied to the camera so the effect follows the player
	CameraPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("CameraPostProcess"));
	CameraPostProcess->SetupAttachment(FirstPersonCamera);
	CameraPostProcess->bUnbound = true;       // Always apply, regardless of volume bounds
	CameraPostProcess->BlendWeight = 1.0f;
	CameraPostProcess->Priority = 1.0f;

	// Default-load the materials and shake classes by path. Safe if the asset is missing —
	// the FObjectFinder simply returns null and the post-process becomes a no-op.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> VHSFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Materials/Instances/PostProcess/MI_OldVHS.MI_OldVHS"));
	if (VHSFinder.Succeeded()) { VHSBaseMaterial = VHSFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> NoiseFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Materials/Instances/PostProcess/MI_Noise.MI_Noise"));
	if (NoiseFinder.Succeeded()) { NoiseBaseMaterial = NoiseFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> AberrFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Materials/Instances/PostProcess/MI_ChromaticAbberation.MI_ChromaticAbberation"));
	if (AberrFinder.Succeeded()) { AberrationBaseMaterial = AberrFinder.Object; }

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> IdleShakeFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Blueprints/CameraShakes/BP_CameraShake_Idle"));
	if (IdleShakeFinder.Succeeded()) { IdleCameraShakeClass = IdleShakeFinder.Class; }

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> WalkShakeFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Blueprints/CameraShakes/BP_CameraShake_Walk"));
	if (WalkShakeFinder.Succeeded()) { WalkCameraShakeClass = WalkShakeFinder.Class; }

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> RunShakeFinder(
		TEXT("/Game/Bodycam_VHS_Effect/Blueprints/CameraShakes/BP_CameraShake_Run"));
	if (RunShakeFinder.Succeeded()) { RunCameraShakeClass = RunShakeFinder.Class; }
}

void ALSCharacter::CreateInputActions()
{
	if (!MoveAction)
	{
		MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
		MoveAction->ValueType = EInputActionValueType::Axis2D;
	}
	if (!LookAction)
	{
		LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
		LookAction->ValueType = EInputActionValueType::Axis2D;
	}
	if (!JumpAction)
	{
		JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
		JumpAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!SprintAction)
	{
		SprintAction = NewObject<UInputAction>(this, TEXT("IA_Sprint"));
		SprintAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!CrouchAction)
	{
		CrouchAction = NewObject<UInputAction>(this, TEXT("IA_Crouch"));
		CrouchAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!AttackAction)
	{
		AttackAction = NewObject<UInputAction>(this, TEXT("IA_Attack"));
		AttackAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!InteractAction)
	{
		InteractAction = NewObject<UInputAction>(this, TEXT("IA_Interact"));
		InteractAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!DefaultMappingContext)
	{
		DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Gameplay"));

		// === KEYBOARD: WASD ===
		// W - Forward
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::W);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}
		// S - Backward
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::S);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Swizzle);
			Mapping.Modifiers.Add(Negate);
		}
		// D - Right
		{
			DefaultMappingContext->MapKey(MoveAction, EKeys::D);
		}
		// A - Left
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::A);
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Negate);
		}

		// === KEYBOARD: ARROW KEYS ===
		// Up Arrow - Forward
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::Up);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}
		// Down Arrow - Backward
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::Down);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Swizzle);
			Mapping.Modifiers.Add(Negate);
		}
		// Right Arrow
		{
			DefaultMappingContext->MapKey(MoveAction, EKeys::Right);
		}
		// Left Arrow
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::Left);
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Negate);
		}

		// === GAMEPAD: LEFT STICK - Movement ===
		// Left stick X (right/left)
		{
			DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftX);
		}
		// Left stick Y (forward/back)
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftY);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}

		// === MOUSE LOOK ===
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Negate->bX = false;
			Negate->bY = true;
			Negate->bZ = false;
			Mapping.Modifiers.Add(Negate);
		}

		// === GAMEPAD: RIGHT STICK - Look ===
		// Right stick X (yaw)
		{
			DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightX);
		}
		// Right stick Y (pitch)
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightY);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}

		// === JUMP ===
		DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
		DefaultMappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom); // A / Cross

		// === SPRINT ===
		DefaultMappingContext->MapKey(SprintAction, EKeys::LeftShift);
		DefaultMappingContext->MapKey(SprintAction, EKeys::Gamepad_LeftThumbstick); // L3 click

		// === CROUCH ===
		DefaultMappingContext->MapKey(CrouchAction, EKeys::LeftControl);
		DefaultMappingContext->MapKey(CrouchAction, EKeys::Gamepad_RightThumbstick); // R3 click

		// === ATTACK ===
		DefaultMappingContext->MapKey(AttackAction, EKeys::LeftMouseButton);
		DefaultMappingContext->MapKey(AttackAction, EKeys::Gamepad_RightTrigger); // RT / R2

		// === INTERACT ===
		DefaultMappingContext->MapKey(InteractAction, EKeys::E);
		DefaultMappingContext->MapKey(InteractAction, EKeys::Gamepad_FaceButton_Left); // X / Square
	}
}

void ALSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure input actions exist
	CreateInputActions();

	// Add mapping context to input subsystem
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 1);
			}
		}
	}

	// Bind death delegate
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ALSCharacter::OnDeath);
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CurrentStamina = MaxStamina;

	// Auto-equip the starter weapon if no other has been set on the combat component
	if (CombatComponent && !CombatComponent->HasWeapon() && StarterUnarmedWeaponClass)
	{
		CombatComponent->EquipWeapon(StarterUnarmedWeaponClass);
	}

	// Bodycam VHS post-process pipeline + idle camera shake
	InitializePostProcess();

	// Fire the intro line(s) so the protagonist's voice greets the player
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			FTimerHandle Tmp1, Tmp2, Tmp3, Tmp4;
			GetWorldTimerManager().SetTimer(Tmp1, [Narr]() { if (Narr) Narr->RequestLine(FName("intro_1")); }, 1.0f, false);
			GetWorldTimerManager().SetTimer(Tmp2, [Narr]() { if (Narr) Narr->RequestLine(FName("intro_2")); }, 6.0f, false);
			GetWorldTimerManager().SetTimer(Tmp3, [Narr]() { if (Narr) Narr->RequestLine(FName("intro_3")); }, 12.5f, false);
			GetWorldTimerManager().SetTimer(Tmp4, [Narr]() { if (Narr) Narr->RequestLine(FName("intro_4")); }, 19.0f, false);
		}
	}

	NextAmbientLineTime = GetWorld()->GetTimeSeconds() + 30.0f;
}

void ALSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Kill plane - respawn if fallen below the level
	if (GetActorLocation().Z < -500.0f)
	{
		SetActorLocation(FVector(800.0f, 800.0f, 100.0f));
		GetCharacterMovement()->StopMovementImmediately();
	}

	// Stamina: drain while sprinting + actually moving, regen otherwise
	const bool bIsActuallyMoving = GetCharacterMovement()->Velocity.SizeSquared2D() > 100.0f;
	const float Now = GetWorld()->GetTimeSeconds();

	if (bIsSprinting && bIsActuallyMoving)
	{
		CurrentStamina = FMath::Max(0.0f, CurrentStamina - StaminaDrainPerSecond * DeltaTime);
		TimeOfLastStaminaUse = Now;
		if (CurrentStamina <= 0.0f)
		{
			HandleSprintStop();
		}
	}
	else if (Now - TimeOfLastStaminaUse >= StaminaRegenDelay)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRegenPerSecond * DeltaTime);
	}

	// Periodically pick a calm/atmospheric line — but only when not in danger
	if (Now >= NextAmbientLineTime)
	{
		NextAmbientLineTime = Now + FMath::FRandRange(22.0f, 38.0f);
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
			{
				if (!Narr->IsInDanger())
				{
					Narr->RequestRandomCalmLine();
				}
			}
		}
	}

	UpdateCameraShakeForMovement();
	UpdatePostProcessIntensity(DeltaTime);
}

void ALSCharacter::InitializePostProcess()
{
	if (!CameraPostProcess)
	{
		return;
	}

	// Add the static VHS overlay at full weight — it's the base "this is a bodycam" look
	if (VHSBaseMaterial)
	{
		FWeightedBlendable VHSBlend;
		VHSBlend.Object = VHSBaseMaterial;
		VHSBlend.Weight = 1.0f;
		CameraPostProcess->Settings.WeightedBlendables.Array.Add(VHSBlend);
	}

	// Noise & chromatic aberration get dynamic instances so we can scale their
	// intensity at runtime based on the narrative danger state.
	if (NoiseBaseMaterial)
	{
		NoiseMID = UMaterialInstanceDynamic::Create(NoiseBaseMaterial, this);
		if (NoiseMID)
		{
			FWeightedBlendable NoiseBlend;
			NoiseBlend.Object = NoiseMID;
			NoiseBlend.Weight = 0.6f;       // Calm baseline
			CameraPostProcess->Settings.WeightedBlendables.Array.Add(NoiseBlend);
		}
	}

	if (AberrationBaseMaterial)
	{
		AberrationMID = UMaterialInstanceDynamic::Create(AberrationBaseMaterial, this);
		if (AberrationMID)
		{
			FWeightedBlendable AberrBlend;
			AberrBlend.Object = AberrationMID;
			AberrBlend.Weight = 0.4f;       // Calm baseline
			CameraPostProcess->Settings.WeightedBlendables.Array.Add(AberrBlend);
		}
	}

	// Start the idle shake immediately so handheld bob is always present
	if (IdleCameraShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->ClientStartCameraShake(IdleCameraShakeClass, 1.0f);
			CurrentShakeClass = IdleCameraShakeClass;
		}
	}
}

void ALSCharacter::UpdateCameraShakeForMovement()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC)
	{
		return;
	}

	const float Speed2D = GetCharacterMovement()->Velocity.Size2D();
	TSubclassOf<UCameraShakeBase> Desired = IdleCameraShakeClass;
	if (Speed2D > 50.0f)
	{
		Desired = (bIsSprinting && RunCameraShakeClass) ? RunCameraShakeClass : WalkCameraShakeClass;
	}

	if (Desired && Desired != CurrentShakeClass)
	{
		if (CurrentShakeClass)
		{
			PC->ClientStopCameraShake(CurrentShakeClass, true);
		}
		PC->ClientStartCameraShake(Desired, 1.0f);
		CurrentShakeClass = Desired;
	}
}

void ALSCharacter::UpdatePostProcessIntensity(float DeltaTime)
{
	if (!NoiseMID && !AberrationMID)
	{
		return;
	}

	// Fetch danger state from the narrative subsystem and lerp toward it so transitions
	// don't snap. In danger: noise/aberration spike. Calm: relaxed baseline.
	float TargetDanger = 0.0f;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULSNarrativeSubsystem* Narr = GI->GetSubsystem<ULSNarrativeSubsystem>())
		{
			TargetDanger = Narr->IsInDanger() ? 1.0f : 0.0f;
		}
	}
	CurrentDangerBlend = FMath::FInterpTo(CurrentDangerBlend, TargetDanger, DeltaTime, 2.5f);

	// Also factor in low health → more visual stress
	float HealthStress = 0.0f;
	if (HealthComponent && HealthComponent->GetMaxHealth() > 0.0f)
	{
		const float HP = HealthComponent->GetHealthPercent();
		HealthStress = FMath::Clamp(1.0f - (HP / 0.5f), 0.0f, 1.0f); // ramps when HP < 50%
	}

	const float CombinedStress = FMath::Clamp(CurrentDangerBlend + HealthStress * 0.7f, 0.0f, 1.0f);

	// Try several common parameter names; the asset author's MIs may use any of them.
	// Setting an unknown parameter is harmless — the engine ignores it.
	if (NoiseMID)
	{
		const float NoiseIntensity = FMath::Lerp(0.35f, 1.15f, CombinedStress);
		NoiseMID->SetScalarParameterValue(TEXT("Intensity"), NoiseIntensity);
		NoiseMID->SetScalarParameterValue(TEXT("Strength"), NoiseIntensity);
		NoiseMID->SetScalarParameterValue(TEXT("Amount"), NoiseIntensity);
	}
	if (AberrationMID)
	{
		const float AberrIntensity = FMath::Lerp(0.4f, 1.6f, CombinedStress);
		AberrationMID->SetScalarParameterValue(TEXT("Intensity"), AberrIntensity);
		AberrationMID->SetScalarParameterValue(TEXT("Strength"), AberrIntensity);
		AberrationMID->SetScalarParameterValue(TEXT("Amount"), AberrIntensity);
	}
}

void ALSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Create input actions before binding (SetupPlayerInputComponent runs before BeginPlay)
	CreateInputActions();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALSCharacter::HandleMove);

		if (LookAction)
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALSCharacter::HandleLook);

		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ALSCharacter::HandleJump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ALSCharacter::HandleStopJump);
		}

		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &ALSCharacter::HandleSprintStart);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALSCharacter::HandleSprintStop);
		}

		if (CrouchAction)
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ALSCharacter::HandleCrouchToggle);

		if (AttackAction)
		{
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &ALSCharacter::HandleAttack);
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Completed, this, &ALSCharacter::HandleStopAttack);
		}

		if (InteractAction)
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ALSCharacter::HandleInteract);
	}
}

float ALSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(ActualDamage);
	}

	return ActualDamage;
}

void ALSCharacter::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALSCharacter::HandleLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X * LookSensitivity);
	AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
}

void ALSCharacter::HandleJump()
{
	Jump();
}

void ALSCharacter::HandleStopJump()
{
	StopJumping();
}

void ALSCharacter::HandleSprintStart()
{
	if (CurrentStamina <= 5.0f)
	{
		// Refuse to start sprinting if exhausted; player has to wait for regen
		return;
	}
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ALSCharacter::HandleSprintStop()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ALSCharacter::HandleCrouchToggle()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ALSCharacter::HandleAttack()
{
	if (CombatComponent)
	{
		CombatComponent->StartFire();
	}
}

void ALSCharacter::HandleStopAttack()
{
	if (CombatComponent)
	{
		CombatComponent->StopFire();
	}
}

void ALSCharacter::HandleInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void ALSCharacter::OnDeath()
{
	UE_LOG(LogLiminalSpaces, Log, TEXT("Player died"));

	DisableInput(Cast<APlayerController>(Controller));

	GetCharacterMovement()->DisableMovement();

	// Notify game mode
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (ALSGameMode* GM = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->RestartMap(PC);
		}
	}
}
