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
#include "DrawDebugHelpers.h"
#include "Projectile.h"

//#include "NiagaraFunctionLibrary.h"


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
	//changed this to false for aiming testing
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.f, 2160.f, 0.f); // ...at this rotation rate
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
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AExoCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &AExoCharacter::EndShoot);

}

void AExoCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}

void AExoCharacter::Dash()
{
	if (DashOffCooldown && CanDash && !InDash)
	{
		CanDash = false;
		DashOffCooldown = false;
		InDash = true;
		
		SetDashAxis();

		CurrentVelocity = GetVelocity();
		const float LaunchMultiplier = 3000.f;
		const float YVelocity = -DashRightAxis;
		const float ZVelocity = DashUpAxis;
		LaunchVelocity = FVector(0, YVelocity, ZVelocity);
		LaunchVelocity.Normalize();
		LaunchVelocity *= LaunchMultiplier;

		LaunchCharacter(LaunchVelocity, true, true);
		//GetCharacterMovement()->AirControl = 100.f;
		//UNiagaraFunctionLibrary::SpawnSystemAttached(ThrusterEffect, GetMesh(), TEXT("ThrusterSocket"), FVector(1), FRotator(1), EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::AutoRelease, true);

		GetWorldTimerManager().SetTimer(DashEndTimer, this, &AExoCharacter::EndDash, 0.15);
		GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &AExoCharacter::ResetCanDash, DashCooldown);
	}
}

void AExoCharacter::SetDashAxis()
{
	if (UpAxis < DashAngleThreshold)
	{
		DashUpAxis = SideDashCompensation;
		if (GetActorForwardVector().Y <= 0)
		{
			DashRightAxis = 1.f;
		} 
		else
		{
			DashRightAxis = -1.f;
		}
		
	}
	else
	{
		DashUpAxis = 1.f;

		DashRightAxis = 0.f;
	}

	//overwrite DashRightAxis if over threshold
	if (RightAxis > DashAngleThreshold)
		DashRightAxis = 1.f;
	else if (RightAxis < -DashAngleThreshold)
		DashRightAxis = -1.f;
}

void AExoCharacter::EndDash()
{
	InDash = false;
	LaunchCharacter(LaunchVelocity / 1.5, true, true);
	GetCharacterMovement()->AirControl = 1.f;
}

void AExoCharacter::ResetCanDash()
{
	DashOffCooldown = true;
}

void AExoCharacter::Reset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AExoCharacter::SetUpAxis(float Value)
{
	UpAxis = Value;
}

void AExoCharacter::SetRightAxis(float Value)
{
	RightAxis = Value;
}

float AExoCharacter::GetDashUpAxis()
{
	return DashUpAxis;
}

float AExoCharacter::GetDashRightAxis()
{
	return DashRightAxis;
}

bool AExoCharacter::GetInDash()
{
	return InDash;
}

float AExoCharacter::GetSideDashCompensation()
{
	return SideDashCompensation;
}

void AExoCharacter::Shoot()
{
	IsShooting = 1.f;
	//UE_LOG(LogTemp, Warning, TEXT("is shooting"));
	GetWorldTimerManager().SetTimer(FireTimer, this, &AExoCharacter::Fire, FireRate, true);

	GetWorldTimerManager().ClearTimer(StopAimingTimer);
}

void AExoCharacter::EndShoot()
{
	//UE_LOG(LogTemp, Warning, TEXT("is done shooting"));

	GetWorldTimerManager().ClearTimer(FireTimer);

	GetWorldTimerManager().SetTimer(StopAimingTimer, this, &AExoCharacter::StopAiming, StopAimingRate, false);
}

void AExoCharacter::StopAiming()
{
	IsShooting = 0.f;
	GetWorldTimerManager().ClearTimer(StopAimingTimer);
}

float AExoCharacter::GetIsShooting()
{
	return IsShooting;
}

void AExoCharacter::Fire()
{
	if (!InDash) {
		UE_LOG(LogTemp, Warning, TEXT("FIRE"));
		FVector Location = GetMesh()->GetSocketLocation("EAR_Socket");
		//DrawDebugSphere(GetWorld(), EARSocketLocation, 20.f, 10.f, FColor::Red, false, 3); 
		FRotator Rotation = GetMesh()->GetSocketRotation("EAR_Socket");
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	}
	
}