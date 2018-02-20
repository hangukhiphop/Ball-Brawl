// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "InertialMovementComponent.h"
#include "OrbitalMovementComponent.h"
#include "PlanarRotatingMovementComponent.h"
#include "BrawlBall.generated.h"

class ABallBoy; 

UCLASS()
class BALLBRAWL_API ABrawlBall : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	ABrawlBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FVector GetVelocity() const;

	void OnCatch(USceneComponent* const CatcherLocation);
	void SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse);
	
	UFUNCTION()
	void OnThrow(const int Color, const FVector& Direction);

private:

	UFUNCTION()
	void OnHit(AActor * SelfActor, AActor * OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBounce(const FHitResult& Hit);
	
public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Collision)
	USphereComponent* BoundingSphere;

protected:

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	UInertialMovementComponent* InertialMovementComponent;

};
