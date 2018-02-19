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
	
	PivotTranslation = FVector::ZeroVector;
	if (ShouldLerp)
	{
		UE_LOG(LogClass, Warning, TEXT("Tick RotationRate: %d %f %f %f"), (int)GetOwnerRole(), RotationRate.Yaw, RotationRate.Pitch, RotationRate.Roll);
		FVector Forward = UpdatedComponent->GetForwardVector();
		if (Forward == LerpForward)
		{
			ShouldLerp = false;
			//RotationRate = FRotator::ZeroRotator;
		}
	}
}

bool UOrbitalMovementComponent::MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	Super::MoveUpdatedComponent(Delta, NewRotation, bSweep, OutHit, Teleport);
	return true;
}

void UOrbitalMovementComponent::SetAngularSpeed(float Speed)
{	
	float Rate = Speed * BaseAngularSpeed;
	FVector Angle = FVector::RightVector * Rate;
	RotationRate = FRotator::MakeFromEuler(Angle); 
	UE_LOG(LogClass, Warning, TEXT("Set speed: %d %f %f %f"), (int)GetOwnerRole(), RotationRate.Yaw, RotationRate.Pitch, RotationRate.Roll);
	if (GetOwnerRole() == ROLE_Authority)
	{
		NetMulticast_SetAngularSpeed(Angle);
	}	
}

void UOrbitalMovementComponent::SetLerpForward(const FVector& NewForward)
{
	if (NewForward == LerpForward || NewForward == UpdatedComponent->GetForwardVector()) { return; }

	LerpForward = NewForward;
	SetAngularSpeed(1);
	ShouldLerp = true;

	UE_LOG(LogClass, Warning, TEXT("SetLerpForward: %d"), (int)GetOwnerRole());
	if (GetOwnerRole() == ROLE_Authority)
	{
		UE_LOG(LogClass, Warning, TEXT("Shit: %d"), (int)GetOwnerRole());
		NetMulticast_SetLerpForward(NewForward);
	}
}

void UOrbitalMovementComponent::NetMulticast_SetLerpForward_Implementation(const FVector & NewForward)
{
	UE_LOG(LogClass, Warning, TEXT("SetLerpForward: %d"), (int)GetOwnerRole());
	SetLerpForward(NewForward);
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


