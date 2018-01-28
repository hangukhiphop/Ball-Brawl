// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "OrbitalMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BALLBRAWL_API UOrbitalMovementComponent : public URotatingMovementComponent
{
	GENERATED_BODY()
public:
	UOrbitalMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep = false, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None);

	void SetAngularSpeed(float Speed);
	void UpdatePivotTranslation(const FVector& NewPivotTranslation);



private:

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetAngularSpeed(const FVector& Speed);

	UFUNCTION(NetMulticast, Reliable)//, WithValidation)
	void NetMulticast_UpdatePivotTranslation(const FVector& NewPivotTranslation);

private:

	//Persistent rotation speed when rotating
	UPROPERTY(EditDefaultsOnly)
	float BaseAngularSpeed;	

	UPROPERTY(Replicated)
	FVector RepPivotTranslation;

	UPROPERTY(Replicated)
	FRotator RepRotationRate;
};
