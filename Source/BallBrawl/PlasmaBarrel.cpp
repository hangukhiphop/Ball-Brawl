// Fill out your copyright notice in the Description page of Project Settings.

#include "PlasmaBarrel.h"


// Sets default values
APlasmaBarrel::APlasmaBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	RootComponent = CapsuleComponent;
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlasmaBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlasmaBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

