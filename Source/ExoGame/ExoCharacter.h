// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ExoCharacter.generated.h"

class UNiagaraSystem;

UCLASS()
class EXOGAME_API AExoCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Combat");
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ThrusterEffect;

	FVector CurrentVelocity;
	FVector LaunchVelocity;

	FTimerHandle DashEndTimer;
	FTimerHandle DashCooldownTimer;
	float DashCooldown = 1.f;
	bool CanDash = true;
	bool DashOffCooldown = true;
	bool InDash = false;
	
	float UpAxis = 0.f;
	float RightAxis = 0.f;
	float DashUpAxis = 0.f;
	float DashRightAxis = 0.f;        
	float SideDashCompensation = 0.2f;
	float DashAngleThreshold = 0.33f;

	float IsShooting = 0.f;	
	FTimerHandle FireTimer;
	float FireRate = 0.025f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for dash ability */
	void Dash();
	void SetDashAxis();
	void EndDash();
	void ResetCanDash();

	void Reset();

	void SetUpAxis(float Value);
	void SetRightAxis(float Value);

	void Shoot();
	void EndShoot();	 
	void Fire();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	
	// Sets default values for this character's properties
	AExoCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	UFUNCTION(BlueprintPure)
	bool GetInDash();

	UFUNCTION(BlueprintPure)
	float GetDashUpAxis();
	
	UFUNCTION(BlueprintPure)
	float GetDashRightAxis();

	UFUNCTION(BlueprintPure)
	float GetSideDashCompensation();

	UFUNCTION(BlueprintPure)
	float GetIsShooting();
};
