// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UTransformDispatch();
	virtual ~UTransformDispatch() override;

public:
	virtual void BeginDestroy() override;
	//THIS CREATES A COPY FOR THE SHADOW BUT UPDATES THE SHADOW EVERY TICK.
	//THIS IS NOT CHEAP.
	void RegisterObjectToShadowTransform(ObjectKey Target, FTransform3d* Original);

	//OBJECT TO TRANSFORM MAPPING IS CALLED FROM MANY THREADS
	//We can't touch the uobjects, but the ftransforms are simply PODs.
	//by modifying a shadow transform rather than the actual transform, we can avoid a data contention.
	//by using a gamesim transform AND a separate gamedisplay transform, we can actually do this. 
	//this likely needs to be a write-safe conc data structure for true speed.
	//we might be able to completely remove the risk by parenting the display transform to the shadow transform, but
	// that has its own problems.
	//I'm considering GrowOnlyLockFreeHash.h
	//temporarily, I'm just locking and prayin', prayin and lockin'. 
	//todo: add proper shadowing either with a conserved transform (OUGH) or something clever. good luck.
	TSharedPtr< TMap<ObjectKey, RealAndShadowTransform>> ObjectToTransformMapping;

	FTransform3d*  GetTransformShadowByObjectKey(ObjectKey Target, ArtilleryTime Now);
	template <class TransformQueuePTR>
	void ApplyShadowTransforms(TransformQueuePTR TransformUpdateQueue);
	//this is about as safe as eating live hornets right now.


	//BEGIN OVERRIDES
	
	virtual void Deinitialize() override;
	virtual void FinishDestroy() override;
	virtual TStatId GetStatId() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	virtual void PostLoad() override;
	virtual void Tick(float DeltaTime) override;
};
