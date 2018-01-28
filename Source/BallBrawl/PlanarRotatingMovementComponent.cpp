// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanarRotatingMovementComponent.h"
#include "GameFramework/Actor.h"

#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

UPlanarRotatingMovementComponent::UPlanarRotatingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bConstrainToPlane = true;
	PlaneConstraintNormal = FVector::RightVector;
	TargetForward = FVector::ZeroVector;
	TargetRotation = FQuat::Identity;
}

void UPlanarRotatingMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Velocity = FVector::ZeroVector;
	//Pivot = FVector::ZeroVector;
}

void UPlanarRotatingMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlanarRotatingMovementComponent, AngularVelocity);
	DOREPLIFETIME(UPlanarRotatingMovementComponent, Pivot);
	DOREPLIFETIME(UPlanarRotatingMovementComponent, ShouldLerpForward);
	DOREPLIFETIME(UPlanarRotatingMovementComponent, TargetRotation);
}

void UPlanarRotatingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (TargetForward.IsZero() || TargetRotation.IsIdentity()) { return; }
	
	FVector DeltaLocation;
	FQuat NewRotation;

	float DeltaAngle = FMath::DegreesToRadians(AngularVelocity * DeltaTime);
	FQuat OldRotation = UpdatedComponent->GetComponentQuat();	
	RotateAngle(DeltaAngle, DeltaLocation, NewRotation);

	float AngDist = OldRotation.AngularDistance(TargetRotation); 
	UE_LOG(LogClass, Warning, TEXT("AngDist1: %f"), AngDist);
	float Threshold = FMath::Abs(DeltaAngle);
	FVector NewForward = NewRotation.RotateVector(FVector::ForwardVector);

	const FVector PivotLocation = Pivot->GetComponentLocation();
	FVector DrawStart = PivotLocation;// +FVector::RightVector;
	DrawDebugLine(GetWorld(), DrawStart, DrawStart + TargetForward * 100.0f, FColor::Red, false, -1.0f, 1, 3.0f);
	DrawStart = UpdatedComponent->GetComponentLocation();
	DrawDebugLine(GetWorld(), DrawStart, DrawStart + NewForward * 100.0f, FColor::Cyan, false, -1.0f, 1, 3.0f);

	AngDist = FMath::Acos(FVector::DotProduct(NewForward, TargetForward));
	UE_LOG(LogClass, Warning, TEXT("AngDist2: %f"), AngDist);
	//UE_LOG(LogClass, Warning, TEXT("TargetForward: %f %f, TargetRotationVector: %f %f"), TargetForward.X, TargetForward.Z, TargetRotation.Vector().X, TargetRotation.Vector().Z)
	//UE_LOG(LogClass, Warning, TEXT("OldRotationVector: %f %f, AngDist: %f, Threshold: %f"), OldForward X, OldForward.Z, AngDist, Threshold)
	/*if (AngDist <= 2 * Threshold)
	{
		NewRotation = TargetRotation; 
		DeltaLocation = OldRotation.RotateVector(PivotLocation) - NewRotation.RotateVector(PivotLocation);
		TargetForward = FVector::ZeroVector;
		TargetRotation = FQuat::Identity;
	}*/
	//NewRotation = OldRotation;
	//DeltaLocation = FVector::ZeroVector;
	MoveUpdatedComponent(DeltaLocation, NewRotation, false);
}

bool UPlanarRotatingMovementComponent::MoveUpdatedComponent(const FVector & Delta, const FQuat & NewRotation, bool bSweep, FHitResult * OutHit, ETeleportType Teleport)
{
	Super::MoveUpdatedComponent(Delta, NewRotation, bSweep, OutHit, Teleport);

	return true;
}

void UPlanarRotatingMovementComponent::SetAngularVelocity(const float NewAngularVelocity)
{
	AngularVelocity = NewAngularVelocity;
}

void UPlanarRotatingMovementComponent::SetTargetRotation(const FVector& Forward)
{
	const FVector PivotLocation = Pivot->GetComponentLocation();
	FQuat NewTarget = Forward.ToOrientationQuat() * (-PivotLocation).ToOrientationQuat().Inverse();
	if(CheckTargetRotation(NewTarget))
	{
		return;
	}

	TargetForward = Forward;
	UE_LOG(LogClass, Warning, TEXT("TargetForward: %f, %f"), TargetForward.X, TargetForward.Z);
	TargetRotation = TargetForward.ToOrientationQuat() * (-PivotLocation).ToOrientationQuat().Inverse();

	FVector Cross = FVector::CrossProduct(UpdatedComponent->GetForwardVector(), Forward);
	float Direction = 1.0f;
	if (FVector::DotProduct(Cross, PlaneConstraintNormal) > 0)
	{
		Direction = -1.0f;
	}
	SetAngularVelocity(Direction * BaseAngularSpeed);



}

void UPlanarRotatingMovementComponent::SetPivot(USceneComponent* SceneComponent)
{	
	Pivot = SceneComponent; //UpdatedComponent->GetComponentLocation() - NewPivot.Size() * FVector::ForwardVector;
	
	//UE_LOG(LogClass, Warning, TEXT("Pivot %f %f"), Pivot.X, Pivot.Z);
}

void UPlanarRotatingMovementComponent::RotateAngle(const float DeltaAngle, FVector& DeltaLocation, FQuat& NewRotation)
{
	const FQuat OldRotation = UpdatedComponent->GetComponentQuat();
	NewRotation = FQuat(PlaneConstraintNormal, DeltaAngle) * OldRotation;

	const FVector PivotLocation = Pivot->GetComponentLocation();
	DeltaLocation = OldRotation.RotateVector(PivotLocation) - NewRotation.RotateVector(PivotLocation); // RotateAngleAxis(DeltaAngle, PlaneConstraintNormal);
}

bool UPlanarRotatingMovementComponent::CheckTargetRotation(const FQuat& Target) const
{
	const FQuat CompQuat = UpdatedComponent->GetComponentQuat();
	return CompQuat == Target;
}

