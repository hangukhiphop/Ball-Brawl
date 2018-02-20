// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "PaperSpriteComponent.h"
#include "Components/SphereComponent.h"
#include "InertialMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BrawlBall.h"

#include "BallBoy.generated.h"

UCLASS()
class BALLBRAWL_API ABallBoy : public APawn
{
	GENERATED_BODY()

#pragma region Constructor(s)

public:
	// Sets default values for this pawn's properties
	ABallBoy();

#pragma endregion Constructor(s)


#pragma region Member Functions

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void MoveX(float Magnitude);

	UFUNCTION(BlueprintNativeEvent)
	void MoveY(float Magnitude);

	UFUNCTION(BlueprintNativeEvent)
	void SetBall_X(float Magnitude);

	UFUNCTION(BlueprintNativeEvent)
	void SetBall_Y(float Magnitude);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsHoldingBall() const;
	void CatchBall(ABrawlBall* Ball);
	void ThrowBall();
	void SetHeldBallDirection(float xOffset, float yOffset);
	
#pragma region Member Functions


#pragma region RPC

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ThrowBall();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ThrowBall();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetHeldBallDirection(FVector Direction, float AngularDistance);


#pragma endregion RPC


#pragma region Component

protected:

	//Root component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Collision)
		USphereComponent* BoundingSphere;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UInertialMovementComponent* InertialMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		USpringArmComponent* SpringArmComponent;

#pragma endregion Component


#pragma region UPROPERTY

protected:

	UPROPERTY(VisibleAnywhere)
	bool bTickMoveBall;

	UPROPERTY(VisibleAnywhere)
	bool bRotateBall;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	uint8 TeamNo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BallRotationSpeed;

	UPROPERTY(VisibleAnywhere)
	float TargetAngularDistance;

	UPROPERTY(VisibleAnywhere)
	FVector TargetBallDirection;

	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere)
	ABrawlBall* HeldBall;


#pragma endregion UPROPERTY


};
