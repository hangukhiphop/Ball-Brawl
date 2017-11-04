// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"

#include "BallBoy.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"


#include "BallBoyController.generated.h"

/**
 * 
 */
UCLASS()
class BALLBRAWL_API ABallBoyController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABallBoyController();

};
