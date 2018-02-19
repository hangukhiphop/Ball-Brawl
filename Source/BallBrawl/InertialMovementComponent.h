// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "InertialMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BALLBRAWL_API UInertialMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	UInertialMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep = false, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None);

	void UpdateVelocity(const FVector& NewVelocity);
	void BounceSurface(const FVector& Normal);

private:

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UpdateVelocity(const FVector& NewVelocity, const FVector& PrevPos);

public:
	
protected:

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector Server_Velocity;

	//Persistent magnitude of Velocity when moving
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed;
};
