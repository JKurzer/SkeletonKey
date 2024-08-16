// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreTypeKeys.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "KeyCarry.generated.h"


//Can we replace this with a gameplay tag based mechanism? that would be most elegant by far.

//this is a simple key-carrier that automatically wires the actorkey up.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKELETONKEY_API UKeyCarry : public UActorComponent
{
	GENERATED_BODY()
	ObjectKey MyObjectKey;
public:
	
	UKeyCarry::UKeyCarry()
	{
		// While we init, we do not tick. This is a "data only" component. gotta be a better way to do this.
		PrimaryComponentTick.bCanEverTick = false;

		// ...
	}

	// Called when the game starts
	virtual void InitializeComponent() override
	{
		Super::BeginPlay();
		if(!IsDefaultSubobject())
		{
			//elegant little thing, really.
			auto val = PointerHash(GetOwner());
			ActorKey TopLevelActorKey = ActorKey(val);
			MyObjectKey = TopLevelActorKey;
		}
		// ...
	
	}

	//will return an invalid object key if it fails.
	static inline ObjectKey Key(AActor* That)
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
	static inline ObjectKey Key(UActorComponent* Me)
	{
		if(Me && Me->GetOwner())
		{
			if(Me->GetOwner()->GetComponentByClass<UKeyCarry>())
			{
				return Me->GetOwner()->GetComponentByClass<UKeyCarry>()->MyObjectKey;
			}
		}
		return ObjectKey();
	}
	
};
//UKeyCarry ended up being a much more generic name that I expected.
//SKELETON is a little silly, but it means that the invocation is often SKELETON::Key(this) which at least
//gives you a hint that the SkeletonKey library is involved at a glance.
typedef UKeyCarry SKELETON;
