// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "PlanarRotatingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BALLBRAWL_API UPlanarRotatingMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlanarRotatingMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool MoveUpdatedComponent(const FVector& Delta, const FQuat& NewRotation, bool bSweep = false, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None);
	
	void SetAngularVelocity(const float NewAngularVelocity);
	void SetTargetRotation(const FVector& Foward);
	void SetPivot(USceneComponent* SceneComponent);
	void RotateAngle(const float DeltaAngle, FVector& DeltaLocation, FQuat& NewRotation);

	bool CheckTargetRotation(const FQuat& Target) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	float BaseAngularSpeed;

	UPROPERTY(Replicated)
	float AngularVelocity;

	UPROPERTY(Replicated)
	USceneComponent* Pivot;

	UPROPERTY(Replicated)
	float RotationRadius;

	UPROPERTY(Replicated)
	FVector TargetForward;

	UPROPERTY(Replicated)
	bool ShouldLerpForward;

	UPROPERTY(Replicated)
	FQuat TargetRotation;


};
