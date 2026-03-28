#include "Player/LSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputActionValue.h"
#include "Combat/LSHealthComponent.h"
#include "Combat/LSCombatComponent.h"
#include "Level/LSInteractableBase.h"
#include "Player/LSInteractionComponent.h"
#include "Core/LSGameMode.h"
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
}

void ALSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Create input actions programmatically if not assigned via Blueprint
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

	// Create mapping context with key bindings
	if (!DefaultMappingContext)
	{
		DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Gameplay"));

		// W - Forward (Axis2D: need to put value on Y axis)
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::W);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}
		// S - Backward (negative Y)
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::S);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Swizzle);
			Mapping.Modifiers.Add(Negate);
		}
		// D - Right (positive X, default)
		{
			DefaultMappingContext->MapKey(MoveAction, EKeys::D);
		}
		// A - Left (negative X)
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::A);
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Mapping.Modifiers.Add(Negate);
		}

		// Mouse Look
		{
			FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext);
			Negate->bX = false;
			Negate->bY = true;
			Negate->bZ = false;
			Mapping.Modifiers.Add(Negate);
		}

		// Jump - Space
		DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);

		// Sprint - Left Shift
		DefaultMappingContext->MapKey(SprintAction, EKeys::LeftShift);

		// Crouch - Left Ctrl
		DefaultMappingContext->MapKey(CrouchAction, EKeys::LeftControl);

		// Attack - Left Mouse Button
		DefaultMappingContext->MapKey(AttackAction, EKeys::LeftMouseButton);

		// Interact - E
		DefaultMappingContext->MapKey(InteractAction, EKeys::E);
	}

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
}

void ALSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
