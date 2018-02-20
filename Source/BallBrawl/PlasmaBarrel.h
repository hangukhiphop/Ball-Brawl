// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "BrawlBall.h"
#include "PlasmaBarrel.generated.h"

UCLASS()
class BALLBRAWL_API APlasmaBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlasmaBarrel();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnHit(ABrawlBall* Ball, int Damage);
	void OnZeroHealth();

	void SetMaxHealth(int Health);

protected:

	UPROPERTY(VisibleAnywhere)
		UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* CapsuleComponent;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		int MaxHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		int CurrentHealth;
	
};
