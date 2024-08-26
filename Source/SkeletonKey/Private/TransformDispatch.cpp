// Fill out your copyright notice in the Description page of Project Settings.
#include "TransformDispatch.h"
#include "SwarmKine.h"

UTransformDispatch::UTransformDispatch()
{
	
}

UTransformDispatch::~UTransformDispatch()
{
	
}

void UTransformDispatch::RegisterObjectToShadowTransform(ObjectKey Target, TObjectPtr<AActor> Self) const
{
	//explicitly cast to parent type.
	TSharedPtr<Kine> kine = MakeShareable<ActorKine>(new ActorKine(Self, Target));
	ObjectToTransformMapping->Add(Target, kine);
}

void UTransformDispatch::RegisterObjectToShadowTransform(ObjectKey Target, UAUKineManager* Manager) const
{
	//explicitly cast to parent type.
	TSharedPtr<Kine> kine = MakeShareable<SwarmKine>(new SwarmKine(Manager, Target));
	ObjectToTransformMapping->Add(Target, kine);
}

TSharedPtr<Kine> UTransformDispatch::GetKineByObjectKey(ObjectKey Target)
{
	auto ref = ObjectToTransformMapping->Find(Target);
	if(ref)
	{
		return  *ref;
	}
	return nullptr;
}

//actual release happens 
void UTransformDispatch::ReleaseKineByKey(ObjectKey Target)
{
	if(Target)
	{
		auto HoldOpen = ObjectToTransformMapping;
		if(HoldOpen)
		{
			auto ref = HoldOpen->Find(Target);
			if(ref){
				ref->Get()->MyKey = ObjectKey();
			}
		}
	}
}

TOptional<FTransform> UTransformDispatch::CopyOfTransformByObjectKey(ObjectKey Target) 
{
	auto ref = ObjectToTransformMapping->Find(Target);
	if(ref)
	{
		return ref->Get()->CopyOfTransformLike();
	}
	return TOptional<FTransform>();
}

TStatId UTransformDispatch::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTransformDispatch, STATGROUP_Tickables);
}

void UTransformDispatch::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ObjectToTransformMapping = MakeShareable(new KineLookup());
	UE_LOG(LogTemp, Warning, TEXT("Shadow Transforms Subsystem: Online"));
}

void UTransformDispatch::Deinitialize()
{
	Super::Deinitialize();
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