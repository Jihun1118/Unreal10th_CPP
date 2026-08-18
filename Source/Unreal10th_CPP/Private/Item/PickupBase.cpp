// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "NiagaraComponent.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryCommandTypes.h"
#include "Framework/SubSystem/PickupFactorySubsystem.h"
#include "Unreal10th_CPP/Unreal10th_CPP.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
	SetRootComponent(SphereCollision);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	NiagaraComponent->SetupAttachment(SphereCollision);
	//NiagaraComponent->Activate();	// 이 나이아가라 컴포넌트가 가지고 있는 나이아가라 에셋을 재생
	//NiagaraComponent->Deactivate();	// 이 나이아가라 컴포넌트가 가지고 있는 나이아가라 에셋을 재생 중지

}

void APickupBase::InitializePickup(UItemDataAsset* InData)
{
	DataAsset = InData;
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.0f;
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIdle)
	{
		OnUpdateUpdownSpin(DeltaTime);
	}
}

void APickupBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	OnPickup(OtherActor);
}

void APickupBase::OnPickup(AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("[APickupBase] : %s(이)가 %s를 획득했습니다."), 
		InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"), *this->GetName());
	bIdle = false;

	if (IInventoryUserInterface* Inven = Cast<IInventoryUserInterface>(InTarget))
	{
		FInventoryCommand Command = FInventoryCommand::MakeAdd(DataAsset, 1);
		FInventoryCommandResult Result;
		if (!Inven->ExecuteInvectoryCommand(Command, Result))
		{
			UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
			FTransform SpawnTransform = InTarget->GetActorTransform();
			SpawnTransform.AddToTranslation(FVector::UpVector * 300.0f);
			Factory->SpawnPickupAsync(DataAsset, SpawnTransform,
				FOnPickupSpawned::CreateWeakLambda(
					this,
					[this](APickupBase* InSpawned)
					{
						UE_LOG(LogTemp, Log, TEXT("%s가 스폰되었습니다."), *InSpawned->GetName());
						OnFinishPickupEffect();
					}
				));
		}
		else
		{
			OnFinishPickupEffect();
		}
	}

}

void APickupBase::OnUpdatePickupEffect()
{

}

void APickupBase::OnFinishPickupEffect()
{
	Destroy();
}

void APickupBase::OnUpdateUpdownSpin(float InDeltaTime)
{
	if (!IsCurveAssetReady()) return;

	ElapsedTime += InDeltaTime;

	if (UMeshComponent* PickupMesh = GetMesh())
	{
		float Div = FMath::Max(UpDownDuration, 0.001f);
		float Progress = FMath::Fmod(ElapsedTime / Div, 1.0f);
		FVector NewMeshLocation = MeshBaseLocation;
		NewMeshLocation.Z += UpDownCurve->GetFloatValue(Progress) * UpDownHeight;

		PickupMesh->SetRelativeLocation(NewMeshLocation);

		float NewAngle = SpinCurve->GetFloatValue(Progress) * 360.0f;
		PickupMesh->SetRelativeRotation(FRotator(0.0f, NewAngle, 0.0f));
	}
}

UMeshComponent* APickupBase::GetMesh() const
{
	return nullptr;
}

bool APickupBase::IsCurveAssetReady() const
{
	return UpDownCurve != nullptr && SpinCurve != nullptr;
}

