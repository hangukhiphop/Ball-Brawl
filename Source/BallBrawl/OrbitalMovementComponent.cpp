// Fill out your copyright notice in the Description page of Project Settings.

#include "OrbitalMovementComponent.h"
#include "UnrealNetwork.h"

UOrbitalMovementComponent::UOrbitalMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bRotationInLocalSpace = true;
	bConstrainToPlane = true;
	PlaneConstraintNormal = FVector::UpVector;
}

void UOrbitalMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	RotationRate = FRotator::ZeroRotator;
	Velocity = FVector::ZeroVector;
	PivotTranslation = FVector::ZeroVector;
}

void UOrbitalMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOrbitalMovementComponent, RepPivotTranslation);
	DOREPLIFETIME(UOrbitalMovementComponent, RepRotationRate);
}

void UOrbitalMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


bool UOrbitalMovementComponent::MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	Super::MoveUpdatedComponent(Delta, NewRotation, bSweep, OutHit, Teleport);
	return true;
}

void UOrbitalMovementComponent::SetAngularSpeed(float Speed)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		float Rate = Speed * BaseAngularSpeed;
		FVector Angle = FVector::RightVector * Rate;
		RotationRate = FRotator::MakeFromEuler(Angle);
		NetMulticast_SetAngularSpeed(Angle);
	}	
}

void UOrbitalMovementComponent::UpdatePivotTranslation(const FVector& NewPivotTranslation)
{
	PivotTranslation = NewPivotTranslation;
	if (GetOwnerRole() == ROLE_Authority)
	{
		RepPivotTranslation = NewPivotTranslation;
		NetMulticast_UpdatePivotTranslation(NewPivotTranslation);
	}	
}

void UOrbitalMovementComponent::NetMulticast_SetAngularSpeed_Implementation(const FVector& NewPivotTranslation)
{
	RotationRate = FRotator::MakeFromEuler(NewPivotTranslation);
}

void UOrbitalMovementComponent::NetMulticast_UpdatePivotTranslation_Implementation(const FVector& NewPivotTranslation)
{
	PivotTranslation = NewPivotTranslation;
}


