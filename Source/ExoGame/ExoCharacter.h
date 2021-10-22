// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ExoCharacter.generated.h"

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

	FVector CurrentVelocity;
	FVector LaunchVelocity;

	FTimerHandle DashTimer;
	bool CanDash = true;
	bool InDash = false;
	
	float UpAxis;
	float RightAxis;
	float AngleThreshold = 0.33f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for dash ability */
	void Dash();
	void EndDash();

	void Reset();

	void SetUpAxis(float Value);
	void SetRightAxis(float Value);

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
};
