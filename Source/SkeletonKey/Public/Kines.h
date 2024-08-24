#pragma once
#include "SkeletonTypes.h"

class KinePair
{
public:
	virtual ~KinePair() = default;
	ObjectKey MyKey;
	//USE FBUPDATES or FBINPUTS. This is provided for legacy reasons and to make kines useful if you do not use Artillery Barrage.
	FTransform RelativeShadow;
};

class KinematicRef : public KinePair
{
public:
	virtual TOptional<FTransform> CopyOfTransformlike() = 0;
	virtual void SetTransformlike(FTransform Input) = 0;
	virtual void SetLocation( FVector3d Location) = 0;
	virtual void SetRotation (FQuat4d Rotation) = 0;
};

using Kine = KinematicRef;

class ActorKine : public Kine
{
public:
	TWeakObjectPtr<AActor> MySelf;

	virtual TOptional<FTransform> CopyOfTransformlike() override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			return Pin->GetActorTransform();
		}
		return TOptional<FTransform>();
	}

	virtual void SetTransformlike(FTransform Input) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			TUniquePtr<FHitResult> MyResult =MakeUnique<FHitResult>();
			Pin->SetActorTransform(Input, true, MyResult.Get(), ETeleportType::None);
		}
	}

	virtual void SetLocation(FVector3d Location) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			TUniquePtr<FHitResult> MyResult =MakeUnique<FHitResult>();
			Pin->SetActorLocation(Location, true, MyResult.Get(), ETeleportType::None);
		}
	}

	virtual void SetRotation(FQuat4d Rotation) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			TUniquePtr<FHitResult> MyResult =MakeUnique<FHitResult>();
			Pin->SetActorRotation(Rotation, ETeleportType::None);
		}
	}
};

using KineLookup = TMap<ObjectKey, TSharedPtr<Kine>>;