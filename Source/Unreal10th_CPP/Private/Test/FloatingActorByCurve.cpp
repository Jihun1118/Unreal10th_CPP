// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/FloatingActorByCurve.h"

void AFloatingActorByCurve::UpdateFloatingMesh()
{
	if (HeightCurve)
	{
		float Div = FMath::Max(Duration, 0.001f);
		float CurveInput = FMath::Fmod(GetElapsedTime() / Div, 1.0f);
		float CurveValue = HeightCurve->GetFloatValue(CurveInput);
		
		Mesh->SetRelativeLocation(FVector(0, 0, GetHeightOffset() + CurveValue * MoveHeight));
	}
	else
	{
		// 커브가 없으면 부모 움직임 따라가기
		Super::UpdateFloatingMesh();
	}
}
