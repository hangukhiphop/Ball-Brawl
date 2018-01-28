// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "PaperSpriteComponent.h"
#include "Components/SphereComponent.h"
#include "InertialMovementComponent.h"
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

	void MoveX(float Magnitude);
	void MoveY(float Magnitude);
	void SetBall_X(float Magnitude);
	void SetBall_Y(float Magnitude);
	void RotateBall(float Magnitude);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CatchBall(ABrawlBall* Ball);
	void ThrowBall();
	void SetHeldBallDirection(float xOffset, float yOffset);
	
#pragma region Member Functions


#pragma endregion UFUNCTION
private:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CatchBall(ABrawlBall* Ball);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ThrowBall();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSpinDirection(float Magnitude);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetHeldBallDirection(FVector Loc);

#pragma endregion UFUNCTION	


#pragma region UPROPERTY

protected:
	UPROPERTY(EditInstanceOnly)
		UPaperSpriteComponent* SpriteComponent;		

	UPROPERTY(EditDefaultsOnly)
		UInertialMovementComponent* InertialMovementComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		float MaxSpeed = 15.0f;	

public:

	UPROPERTY(EditAnywhere, Category = Collision)
		USphereComponent* BoundingSphere;

	UPROPERTY(EditAnywhere)
		int TeamNo;

	UPROPERTY(Replicated, BlueprintReadOnly)
		ABrawlBall* HeldBall;

#pragma endregion UPROPERTY


private:
	float HeldBallDistance;

};
