// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kines.h"
#include "SkeletonTypes.h"
#include "SwarmKine.h"
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
	void RegisterObjectToShadowTransform(ObjectKey Target, TObjectPtr<AActor> Original) const;
	void RegisterObjectToShadowTransform(ObjectKey Target, UAUKineManager* Manager) const;

	TOptional<Kine> GetKineByObjectKey(ObjectKey Target);
	//OBJECT TO TRANSFORM MAPPING IS CALLED FROM MANY THREADS
	//Unfortunately, we ended up needed to hide an actor ref inside the Kine. This means that it's risky at best
	//to call get transform on a kine off the game thread. This might be an actual blocker. There's a way around it, but I'm not in love with it.
	//todo: add proper shadowing with a **readonly** const copy of the transform after update? That allows us to totally hide barrage
	TSharedPtr< KineLookup> ObjectToTransformMapping;
	
	void ReleaseKineByKey(ObjectKey Target);

	//right now, this is only a helper method, but if we add the read-only copy in the kine itself, we could conceivably
	//use this as a one-frame conserve without a lock. 
	TOptional<FTransform3d> CopyOfTransformByObjectKey(ObjectKey Target);

	//it's not clear if this can be made safe to call off gamethread. It's an unfortunate state of affairs to be sure.
	template <class TransformQueuePTR>
	void ApplyTransformUpdates(TransformQueuePTR TransformUpdateQueue);

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
void UTransformDispatch::ApplyTransformUpdates(TransformQueuePTR TransformUpdateQueue)
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
	
}
