#pragma once

#include "SkeletonTypes.h"

class KineData 
{
public:
	virtual ~KineData() = default;
	FSkeletonKey MyKey; 
	//I don't recommend adding data here, but this base class is provided in case you do not use barrage and need a place to keep shadow or
	//scratch transforms.
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

class ActorKine;

class ActorKine : public Kine
{
	//static inline FActorComponentTickFunction FALSEHOOD_MALEVOLENCE_TRICKERY = FActorComponentTickFunction();
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
			Pin->SetActorLocationAndRotation(Loc, Rot);
			// TODO: evaluate whether this is right or not, but this seems to work
			/**auto Ref = Pin->GetRootComponent();
			if(Ref)
			{
				auto& transform = const_cast<FTransform&>(Ref->GetComponentTransform());
				transform.SetLocation(Loc);
				transform.SetRotation(Rot);
				Ref->MarkRenderTransformDirty();
			}**/
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
			/**
			auto Ref = Pin->GetRootComponent();
			if(Ref)
			{
				auto& transform = const_cast<FTransform&>(Ref->GetComponentTransform());
				transform.SetLocation(Location);
				Ref->MarkRenderTransformDirty();
			}**/
		}
	}

	virtual void SetRotation(FQuat4d Rotation) override
	{
		TObjectPtr<AActor> Pin;
		Pin = MySelf.Get();
		if(Pin)
		{
			Pin->SetActorRotation(Rotation, ETeleportType::None);
			/**
			auto Ref = Pin->GetRootComponent();
			if(Ref)
			{
				auto& transform = const_cast<FTransform&>(Ref->GetComponentTransform());
				transform.SetRotation(Rotation);
				Ref->MarkRenderTransformDirty();
			}**/
		}
	}
};


using KineLookup = TMap<FSkeletonKey, TSharedPtr<Kine>>;
