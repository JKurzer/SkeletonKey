#pragma once
#include "SkeletonTypes.h"

class KineData
{
public:
	virtual ~KineData() = default;
	ObjectKey MyKey; 
	//USE FBUPDATES or FBINPUTS. This is provided for legacy reasons and to make kines useful if you do not use Artillery Barrage.
};

class KinematicRef : public KineData
{
public:
	TOptional<FTransform> CopyOfTransformLike()
	{
		if(MyKey)
		{
			return  CopyOfTransformlike_Impl();
		}
		return TOptional<FTransform>();
	}
	virtual void SetTransformlike(FTransform Input) = 0;
	virtual void SetLocation( FVector3d Location) = 0;
	virtual void SetRotation (FQuat4d Rotation) = 0;
	virtual void SetLocationAndRotation(FVector3d Loc, FQuat4d Rot) = 0;
	bool IsNull() const { return MyKey == 0;}
protected:
	virtual TOptional<FTransform> CopyOfTransformlike_Impl() = 0;
};
//You can assess the broad type of the Kine's semantic meaning with the hidden runtime type infix found in every ObjectKey.
//This constant-time single-layer reflection trick allows runtime typesafety without allowing deep hierarchy.
using Kine = KinematicRef;




class ActorKine : public Kine
{
public:
	TWeakObjectPtr<AActor> MySelf;

	explicit ActorKine(const TWeakObjectPtr<AActor>& MySelf, const ActorKey& Target)
		: MySelf(MySelf)
	{
		MyKey = Target;
	}

	virtual void SetLocationAndRotation(FVector3d Loc, FQuat4d Rot) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			auto Ref = Pin->GetRootComponent();
			if(Ref)
			{
				auto& transform = const_cast<FTransform&>(Ref->GetComponentTransform());
				transform.SetLocation(Loc);
				transform.SetRotation(Rot);
				Ref->MarkRenderStateDirty();
			}
		}
	}

	virtual TOptional<FTransform> CopyOfTransformlike_Impl() override
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
			Pin->SetActorTransform(Input, false, nullptr, ETeleportType::None);
		}
	}

	virtual void SetLocation(FVector3d Location) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			Pin->SetActorLocation(Location, false, nullptr, ETeleportType::None);
		}
	}

	virtual void SetRotation(FQuat4d Rotation) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			Pin->SetActorRotation(Rotation, ETeleportType::None);
		}
	}
};

using KineLookup = TMap<ObjectKey, TSharedPtr<Kine>>;