// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupEffect.h"
#include "Component/StatComponent.h"
#include "Interface/StaminaInterface.h"
#include "Interface/HealthInterface.h"
#include "Interface/StatInterface.h"

void APickupEffect::OnPickup(AActor* InTarget)
{
	Super::OnPickup(InTarget);
	// bImplements이 true면 인터페이스를 구현했다.
	// bool bImplements = OtherActor->Implements<UStaminaInterface>()

	if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
	{
		if (UStatComponent* StatComp = Stat->GetStatComponent())
		{		
			if (Stamina > 0)
			{
				IStaminaInterface::Execute_RecoveryStamina(StatComp, Stamina);
			}
			else if (Stamina < 0)
			{
				IStaminaInterface::Execute_ConsumeStamina(StatComp, -Stamina);
			}

			if (Health > 0)
			{
				IHealthInterface::Execute_HealHealth(StatComp, Health);
			}
			else if (Health < 0)
			{
				IHealthInterface::Execute_DamageHealth(StatComp, -Health);
			}
		}
	}

	// Target이 null이 아니면 인터페이스를 상속받았다(= C++니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우는 체크 불가능)
	// IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);
}
