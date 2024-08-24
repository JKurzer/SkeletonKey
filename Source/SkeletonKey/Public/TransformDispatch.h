// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kines.h"
#include "SkeletonTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "TransformDispatch.generated.h"

/**
 * Transform dispatch is the core use case of the skeleton key system. It allows you to assign a key to an arbitrary
 * transform, effectively letting you create implicit objects and actors. This also lets you later use Artillery to bind
 * attributes to any of these transforms. I recognize that this is a strange model compared to the UE norm, but it's actually
 * not particularly different from the mesh managers, lightweight instances, and proxies, and in fact,
 * I expect we'll integrate with said systems.
 *
 * Effectively, this separates ephemeral data-driven mutations from the UObject type system without breaking type safety.
 */
UCLASS()
class SKELETONKEY_API UTransformDispatch : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	ObjectKey DefaultObjectKey;
	UTransformDispatch();
	virtual ~UTransformDispatch() override;

public:
	//THIS CREATES A COPY FOR THE SHADOW BUT UPDATES THE SHADOW EVERY TICK.
	//THIS IS NOT CHEAP.
	void RegisterObjectToShadowTransform(ObjectKey Target, FTransform3d* Original);

	TOptional<Kine> GetKineByObjectKey(ObjectKey Target);
	//OBJECT TO TRANSFORM MAPPING IS CALLED FROM MANY THREADS
	//Unfortunately, we ended up needed to hide an actor ref inside the Kine. This means that it's risky at best
	//to call get transform on a kine off the game thread. I'm working on a solution for this. This is still touched
	//off thread, mostly in a read op-fashion. Relative Shadow can be safely modified.
	//However, because the kine combines the shadow into the key, it's a little easier to consider what locking scheme
	//we might use. 
	//todo: add proper shadowing either with a conserved transform (OUGH) or something clever. good luck.
	TSharedPtr< KineLookup> ObjectToTransformMapping;

	FTransform3d* GetTransformShadowByObjectKey(ObjectKey Target, ArtilleryTime Now);
	FTransform3d* GetTransformShadowByObjectKey(ObjectKey Target);
	FTransform3d* GetOriginalTransformByObjectKey(ObjectKey Target);

	template <class TransformQueuePTR>
	void ApplyShadowTransforms(TransformQueuePTR TransformUpdateQueue);;
	//this is about as safe as eating live hornets right now.

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//BEGIN OVERRIDES
	
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	virtual void PostLoad() override;
	virtual void Tick(float DeltaTime) override;
};

template <class TransformQueuePTR>
void UTransformDispatch::ApplyShadowTransforms(TransformQueuePTR TransformUpdateQueue)
{
	//process updates from barrage.
	auto HoldOpen = TransformUpdateQueue;

	//MARKED SAFE by knock-out testing.

	//This applies the update from Jolt
	while(HoldOpen && !HoldOpen->IsEmpty())
	{
		auto Update = HoldOpen->Peek();
		if(Update)
		{
			if(TSharedPtr<Kine> BindOriginal = this->GetKineByObjectKey(Update->ObjectKey))
			{
				BindOriginal->SetLocation( UE::Math::TVector<double>(Update->Position));
				BindOriginal->SetRotation(UE::Math::TQuat<double>(Update->Rotation));
			}
			HoldOpen->Dequeue();
		}
	}

	//this applies the shadow transform afterwards.
	//TODO: THIS MUST BE REWORKED.
	for(auto& x : *ObjectToTransformMapping)
	{
		auto& kine = x.Value;
		//if the transform hasn't changed, this can explode. honestly, this can just explode. it's just oofa.
		//we really want the transform delta to be _additive_ but that's gonna take quite a bit more work.
		//good news, it'll be much faster, cause we'll zero the delta instead? I think? I think? rgh.
		//it's not a problem atm. mostly.
		auto current = kine->CopyOfTransformlike();
		if(current.IsSet())
		{
			current->Accumulate(kine->RelativeShadow);
			kine->SetTransformlike(current.GetValue());
		}
		kine->RelativeShadow = FTransform3d::Identity;
	}


}
