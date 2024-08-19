// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkeletonTypes.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "KeyCarry.generated.h"


//Can we replace this with a gameplay tag based mechanism? that would be most elegant by far.

//this is a simple key-carrier that automatically wires the actorkey up.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DefaultToInstanced)
class SKELETONKEY_API UKeyCarry : public UActorComponent
{
	GENERATED_BODY()
	ObjectKey MyObjectKey;
public:
	
	UKeyCarry()
	{
		// While we init, we do not tick. This is a "data only" component. gotta be a better way to do this.
		auto val = PointerHash(GetOwner());
		ActorKey TopLevelActorKey = ActorKey(val);
		MyObjectKey = TopLevelActorKey;
		PrimaryComponentTick.SetTickFunctionEnable(false);
		// ...
	}
	

	//will return an invalid object key if it fails.
	static inline ObjectKey KeyOf(AActor* That)
	{
	if(That)
	{
		if(That->GetComponentByClass<UKeyCarry>())
		{
			return That->GetComponentByClass<UKeyCarry>()->MyObjectKey;
		}
	}
	return ObjectKey();
	}

	//will return an invalid object key if it fails.
	static inline ObjectKey KeyOf(UActorComponent* Me)
	{
		if(Me && Me->GetOwner())
		{
			auto ptr = Me->GetOwner()->GetComponentByClass<UKeyCarry>();
			if(ptr)
			{
				return ptr->MyObjectKey;
			}
		}
		return ObjectKey();
	}
	
};
//UKeyCarry ended up being a much more generic name that I expected.
//SKELETON is a little silly, but it means that the invocation is often SKELETON::Key(this) which at least
//gives you a hint that the SkeletonKey library is involved at a glance.
typedef UKeyCarry SKELETON;
