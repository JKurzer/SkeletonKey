// Fill out your copyright notice in the Description page of Project Settings.


#include "TransformDispatch.h"

UTransformDispatch::UTransformDispatch()
{
	
}

UTransformDispatch::~UTransformDispatch()
{
}

void UTransformDispatch::RegisterObjectToShadowTransform(ObjectKey Target, FTransform3d* Original)
{
	ObjectToTransformMapping->Add(Target, KineSimBind(Original, FTransform3d()));
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

FTransform3d* UTransformDispatch::GetTransformShadowByObjectKey(ObjectKey Target) 
{
	auto ref = ObjectToTransformMapping->Find(Target);
	if(ref)
	{
		return &(ref->Value);
	}
	return nullptr;
}


FTransform3d* UTransformDispatch::GetOriginalTransformByObjectKey(ObjectKey Target) 
{
	auto ref = ObjectToTransformMapping->Find(Target);
	if(ref)
	{
		return (ref->Key);
	}
	return nullptr;
}






TStatId UTransformDispatch::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTransformDispatch, STATGROUP_Tickables);
}


void UTransformDispatch::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	ObjectToTransformMapping = MakeShareable(new TMap<ObjectKey, KineSimBind>);
	
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
