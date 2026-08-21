// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemAction.generated.h"

/**
 * 
 */
// Abstract : 추상클래스로 설정(인스턴스 못만듬. 상속받은 자식만 인스턴스 생성 가능)
// EditInlineNew : 디테일 패널 드랍다운에서 즉시 생성 및 편집 가능
// DefaultToInstanced : 이 클래스를 주소로 가지는 모든 UPROPERTY에 Instanced속성을 기본적으로 부여
// Instanced : 이 객체를 소유자의 하위 인스턴스로 취급(=디스크에 저장 가능 + 딥 카피 보장)
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class UNREAL10TH_CPP_API UItemAction : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "ItemAction")
	void ExecuteAction(AActor* InInstigator, AActor* InTarget);

	// 언리얼 해더 툴 제약상 BlueprintNativeEvent는 순수 가상함수 불가능. 그래서 기본 바디 필수
	virtual void ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget) {}
};
