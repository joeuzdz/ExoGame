// Fill out your copyright notice in the Description page of Project Settings.


#include "ExoCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AExoCharacter::AExoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 1750.f;
	//CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 2160.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 3.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->JumpZVelocity = 1500.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;
}

// Called when the game starts or when spawned
void AExoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GetCharacterMovement()->IsFalling())
	{
		CanDash = true;
	}
}

// Called to bind functionality to input
void AExoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AExoCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("SetUpAxis"), this, &AExoCharacter::SetUpAxis);
	PlayerInputComponent->BindAxis(TEXT("SetRightAxis"), this, &AExoCharacter::SetRightAxis);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &AExoCharacter::Dash);
	PlayerInputComponent->BindAction(TEXT("Reset"), EInputEvent::IE_Pressed, this, &AExoCharacter::Reset);

}

void AExoCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}

void AExoCharacter::Dash()
{
	if (CanDash && !InDash)
	{
		CanDash = false;
		InDash = true;
		CurrentVelocity = GetVelocity();
		const float LaunchMultiplier = 3000.f;
		const float YVelocity = -RightAxis;
		const float ZVelocity = UpAxis;
		LaunchVelocity = FVector(0, YVelocity, ZVelocity);
		LaunchVelocity.Normalize();
		LaunchVelocity *= LaunchMultiplier;

		LaunchCharacter(LaunchVelocity, true, true);


		GetWorldTimerManager().SetTimer(DashTimer, this, &AExoCharacter::EndDash, 0.15);
	}
}

void AExoCharacter::EndDash()
{
	InDash = false;
	LaunchCharacter(LaunchVelocity / 1.5, true, true);
}

void AExoCharacter::Reset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AExoCharacter::SetUpAxis(float Value)
{
	if (Value > AngleThreshold) UpAxis = 1.f;
	else if (Value < -AngleThreshold) UpAxis = -1.f;
	else UpAxis = 0.2f;
}

void AExoCharacter::SetRightAxis(float Value)
{
	if (Value > AngleThreshold) RightAxis = 1.f;
	else if (Value < -AngleThreshold) RightAxis = -1.f;
	else RightAxis = 0.f;
}