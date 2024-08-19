// Fill out your copyright notice in the Description page of Project Settings.


#include "TransformDispatch.h"

UTransformDispatch::UTransformDispatch()
{
	ObjectToTransformMapping = MakeShareable(new TMap<ObjectKey, RealAndShadowTransform>);
}

UTransformDispatch::~UTransformDispatch()
{
}
void UTransformDispatch::RegisterObjectToShadowTransform(ObjectKey Target, FTransform3d* Original)
{
	ObjectToTransformMapping->Add(Target, RealAndShadowTransform(Original, FTransform3d()));
}


FTransform3d* UTransformDispatch::GetTransformShadowByObjectKey(ObjectKey Target, ArtilleryTime Now) 
{
	auto ref = ObjectToTransformMapping->Find(Target);
	if(ref)
	{
		return &(ref->Value);
	}
	return nullptr;
}
template <typename TransformQueuePTR>
void UTransformDispatch::ApplyShadowTransforms(TransformQueuePTR TransformUpdateQueue)
{
	//process updates from barrage.
	auto HoldOpen = TransformUpdateQueue;

	//MARKED SAFE by knock-out testing.
	while(HoldOpen && !HoldOpen->IsEmpty())
	{
		if(auto Update = HoldOpen->Peek())
		{
			
			auto destructure = ObjectToTransformMapping->Find(Update->ObjectKey);
			const auto& bindOriginal = destructure->Key;
			bindOriginal->SetTranslation( UE::Math::TVector<double>(Update->Position));
			bindOriginal->SetRotation(UE::Math::TQuat<double>(Update->Rotation));
			HoldOpen->Dequeue();
		}
	}
	
	for(auto& x : *ObjectToTransformMapping)
	{
		auto& destructure = x.Value;
		const auto& bindConst = destructure.Value;
		//if the transform hasn't changed, this can explode. honestly, this can just explode. it's just oofa.
		//we really want the transform delta to be _additive_ but that's gonna take quite a bit more work.
		//good news, it'll be much faster, cause we'll zero the delta instead? I think? I think? rgh.
		//it's not a problem atm. mostly.
		
		(destructure.Key)->Accumulate(bindConst);
		destructure.Value = FTransform3d::Identity;
	}
}











TStatId UTransformDispatch::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTransformDispatch, STATGROUP_Tickables);
}

void UTransformDispatch::BeginDestroy()
{
	Super::BeginDestroy();
}

void UTransformDispatch::Deinitialize()
{
	Super::Deinitialize();
	
	ObjectToTransformMapping->Empty();
}

void UTransformDispatch::FinishDestroy()
{
	Super::FinishDestroy();
}



void UTransformDispatch::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTransformDispatch::PostInitialize()
{
	Super::PostInitialize();
}

void UTransformDispatch::PostLoad()
{
	Super::PostLoad();
}

void UTransformDispatch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
