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
