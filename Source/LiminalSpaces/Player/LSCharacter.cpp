#include "Player/LSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
