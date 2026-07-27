// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatingActor.h"
#include "FloatingActorByCurve.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API AFloatingActorByCurve : public AFloatingActor
{
	GENERATED_BODY()
	
protected:
	virtual void UpdateFloatingMesh() override;

protected:
	// 움직임을 표현할 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> HeightCurve = nullptr;
};
