#pragma once
#include "SkeletonTypes.h"
#include "Kines.h"
#include "CoreMinimal.h"
#include "InstanceDataTypes.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "SwarmKine.generated.h"

class UObject;

//interface that adds support for owning swarm kines. used for managing many many meshes at a time.
//generally, 
UCLASS()
class SKELETONKEY_API USwarmKineManager : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:
	explicit USwarmKineManager()
		: KeyToMesh(), MeshToKey()
	{
		KeyToMesh = MakeShareable(new TMap<FSkeletonKey, int32>());
		MeshToKey = MakeShareable(new TMap<int32, FSkeletonKey>());
	}

	// No chaos physics for you
	virtual bool ShouldCreatePhysicsState() const override
	{
		return false;
	}
	
	virtual TOptional<FTransform> GetTransformCopy(FSkeletonKey Target)
	{
		auto m = KeyToMesh->Find(Target);
		FTransform ref;
		if(m)
		{
			if (GetInstanceTransform(GetInstanceIndexForId(FPrimitiveInstanceId(*m)), ref, true))
			{
				return ref;
			}
		}
		return TOptional<FTransform>();
	};
	virtual bool SetTransformOnInstance(FSkeletonKey Target, FTransform Update)
	{
		auto m = KeyToMesh->Find(Target);
		if(m)
		{
			return UpdateInstanceTransform(GetInstanceIndexForId(FPrimitiveInstanceId(*m)), Update, true, true, true);
		}
		return false;
	};
	virtual FSkeletonKey GetKeyOfInstance(FPrimitiveInstanceId Target)
	{
		auto m = MeshToKey->Find(Target.Id);
		return m ? *m : FSkeletonKey();
	};

	virtual void AddToMap(FPrimitiveInstanceId MeshId, FSkeletonKey Key)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding ISM instance #%i"), MeshId.Id);
		KeyToMesh->Add(Key, MeshId.Id);
		MeshToKey->Add(MeshId.Id, Key);
	}

	virtual void CleanupInstance(const FSkeletonKey Target)
	{
		auto HoldOpen = KeyToMesh;
		if (HoldOpen)
		{
			auto m = KeyToMesh->FindRef(Target);
			MeshToKey->Remove(m);
			RemoveInstanceById(FPrimitiveInstanceId(m));
			KeyToMesh->Remove(Target);
		}
	}
	
private:
	TSharedPtr<TMap<FSkeletonKey, int32>> KeyToMesh;
	TSharedPtr<TMap<int32, FSkeletonKey>> MeshToKey;
};


class SwarmKine : public Kine
{

	TWeakObjectPtr<USwarmKineManager> MyManager;

public:
	explicit SwarmKine(const TWeakObjectPtr<USwarmKineManager>& MyManager, const FSkeletonKey& MeshInstanceKey)
		: MyManager(MyManager)
	{
		MyKey  = MeshInstanceKey;
	}

	virtual void SetTransformlike(FTransform Input) override
	{
		MyManager->SetTransformOnInstance(MyKey, Input);
	}

	virtual void SetLocationAndRotation(FVector3d Loc, FQuat4d Rot) override
	{
		auto m = MyManager->GetTransformCopy(MyKey);
		if(m.IsSet())
		{
			m->SetLocation(Loc);
			m->SetRotation(Rot);
			MyManager->SetTransformOnInstance(MyKey, m.GetValue());
		}	
	}

	virtual void SetLocation(FVector3d Location) override
	{
		auto m = MyManager->GetTransformCopy(MyKey);
		if(m.IsSet())
		{
			m->SetLocation(Location);
			MyManager->SetTransformOnInstance(MyKey, m.GetValue());
		}	
	}

	virtual void SetRotation(FQuat4d Rotation) override
	{
		auto m = MyManager->GetTransformCopy(MyKey);
		if(m.IsSet())
		{
			m->SetRotation(Rotation);
			MyManager->SetTransformOnInstance(MyKey, m.GetValue());
		}	
	}
	
protected:
	virtual TOptional<FTransform> CopyOfTransformlike_Impl() override
	{
		return MyManager->GetTransformCopy(MyKey);
	}


};
