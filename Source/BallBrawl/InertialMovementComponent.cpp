// Fill out your copyright notice in the Description page of Project Settings.

#include "InertialMovementComponent.h"
#include "UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"

UInertialMovementComponent::UInertialMovementComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PlaneConstraintNormal = FVector::RightVector;
	bWantsInitializeComponent = true;
}

void UInertialMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Server_Velocity = FVector::ZeroVector;
}

void UInertialMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInertialMovementComponent, Server_Velocity);
}

void UInertialMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult HitResult;
	FVector Delta = Server_Velocity * Speed * DeltaTime;
	MoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, &HitResult, ETeleportType::None);
}

bool UInertialMovementComponent::MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	Super::MoveUpdatedComponent(Delta, NewRotation, bSweep, OutHit, Teleport);
	return true;
}

void UInertialMovementComponent::UpdateVelocity(FVector NewVelocity)
{	
	if (NewVelocity == Velocity)
	{
		return;
	}
	Server_Velocity = NewVelocity;
	Velocity = Server_Velocity;
	
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_UpdateVelocity(NewVelocity);
	}
}

void UInertialMovementComponent::Server_UpdateVelocity_Implementation(FVector NewVelocity)
{
	UpdateVelocity(NewVelocity);
}

bool UInertialMovementComponent::Server_UpdateVelocity_Validate(FVector NewVelocity)
{
	return true;
}