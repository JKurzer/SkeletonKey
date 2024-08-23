// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkeletonTypes.h"
#include "TransformDispatch.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "KeyCarry.generated.h"

//this is a simple key-carrier that automatically wires the actorkey up.
//It tries during initialize, and if that fails, it tries again each tick until successful,
//then turns off ticking for itself. Clients should use the Retry_Notify delegate to register
//for notification of success in production code, rather than relying on initialization sequencing.
//Later versions will also set a gameplay tag to indicate that this actor carries a key.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DefaultToInstanced)
class SKELETONKEY_API UKeyCarry : public UActorComponent
{
	GENERATED_BODY()
	ObjectKey MyObjectKey;
public:
	DECLARE_MULTICAST_DELEGATE(ActorKeyIsReady)
	 ActorKeyIsReady Retry_Notify;
	bool isReady = false;
	ObjectKey GetObjectKey()
	{
		return MyObjectKey;
	}
	
	UKeyCarry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		// While we init, we do not tick more than once.. This is a "data only" component. gotta be a better way to do this.

		PrimaryComponentTick.bCanEverTick = true;
		bWantsInitializeComponent = true;
		// ...
	}
	

	void AttemptRegister()
	{
		if(GetWorld())
		{
			if(auto xRef = GetWorld()->GetSubsystem<UTransformDispatch>())
			{
				if(TObjectPtr<AActor> actorRef = GetOwner())
				{
					
					actorRef->UpdateComponentTransforms();
					auto a =  actorRef->GetComponentByClass<UStaticMeshComponent>();
					if(a)
					{
						a->bEvaluateWorldPositionOffset = true;
						a->UpdateInitialEvaluateWorldPositionOffset();
					}
					FTransform3d* transf = const_cast<FTransform3d*>(&actorRef->GetTransform());
					if(transf)
					{
						auto val = PointerHash(GetOwner());
						ActorKey TopLevelActorKey = ActorKey(val);
						MyObjectKey = TopLevelActorKey;
						xRef->RegisterObjectToShadowTransform( MyObjectKey ,transf);
						isReady = true;
						if(Retry_Notify.IsBound())
						{
							Retry_Notify.Broadcast();
						}
						SetComponentTickEnabled(false);
					}
				}
			}
		}
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

	virtual void InitializeComponent() override
	{
		Super::InitializeComponent();
		AttemptRegister();
	};

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override
	{
		//this shouldn't come up often in practice but late initialization can be an issue.
		if(!isReady)
		{
			AttemptRegister();
		}
		else
		{
			//shouldn't come up, but in case a refactor makes this case arise:
			SetComponentTickEnabled(false);
		}
	};

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
	
	virtual void BeginDestroy() override
	{
		Super::BeginDestroy();
		if(GetWorld())
		{
			if(auto xRef = GetWorld()->GetSubsystem<UTransformDispatch>())
			{
				if(auto actorRef = GetOwner())
				{
					auto transf = xRef->GetTransformShadowByObjectKey(MyObjectKey);
					if(transf)
					{
						//TODO:  remove goes here.
					}
				}
			}
		}
		
	};
	
};
//UKeyCarry ended up being a much more generic name that I expected.
//SKELETON is a little silly, but it means that the invocation is often SKELETON::Key(this) which at least
//gives you a hint that the SkeletonKey library is involved at a glance.
typedef UKeyCarry SKELETON;
