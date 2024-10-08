// Fill out your copyright notice in the Description page of Project Settings.
#include "TransformDispatch.h"
#include "SwarmKine.h"

UTransformDispatch::UTransformDispatch()
{
	
}

UTransformDispatch::~UTransformDispatch()
{
	
}

void UTransformDispatch::RegisterObjectToShadowTransform(FSkeletonKey Target, TObjectPtr<AActor> Self) const
{
	//explicitly cast to parent type.
	TSharedPtr<Kine> kine = MakeShareable<ActorKine>(new ActorKine(Self, Target));
	ObjectToTransformMapping->Add(Target, kine);
}

void UTransformDispatch::RegisterObjectToShadowTransform(FSkeletonKey Target, USwarmKineManager* Manager) const
{
	//explicitly cast to parent type.
	TSharedPtr<Kine> kine = MakeShareable<SwarmKine>(new SwarmKine(Manager, Target));
	ObjectToTransformMapping->Add(Target, kine);
}

TSharedPtr<Kine> UTransformDispatch::GetKineByObjectKey(FSkeletonKey Target) const
{
	TSharedPtr<KinematicRef>* ref = ObjectToTransformMapping->Find(Target);
	return ref ? *ref : nullptr;
}

TSharedPtr<ActorKine> UTransformDispatch::GetActorKineByObjectKey(FSkeletonKey Target) const
{
	TSharedPtr<Kine>* ref = ObjectToTransformMapping->Find(Target);
	// TODO: this isn't safe, will probably throw if its not actually an ActorKine
	return ref ? StaticCastSharedPtr<ActorKine>(*ref) : nullptr;
}

TWeakObjectPtr<AActor> UTransformDispatch::GetAActorByObjectKey(FSkeletonKey Target) const
{
	TSharedPtr<ActorKine> ActorKinePtr = GetActorKineByObjectKey(Target);
	return ActorKinePtr.IsValid() ? ActorKinePtr->MySelf : nullptr;
}

//actual release happens 
void UTransformDispatch::ReleaseKineByKey(FSkeletonKey Target)
{
	if(Target)
	{
		auto HoldOpen = ObjectToTransformMapping;
		if(HoldOpen)
		{
			TSharedPtr<Kine>* ref = HoldOpen->Find(Target);
			if(ref && *ref){
				ref->Get()->MyKey = FSkeletonKey();
			}
		}
	}
}

TOptional<FTransform> UTransformDispatch::CopyOfTransformByObjectKey(FSkeletonKey Target) 
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
	SelfPtr = nullptr;
	Super::Deinitialize();
}

void UTransformDispatch::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	SelfPtr = this;
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