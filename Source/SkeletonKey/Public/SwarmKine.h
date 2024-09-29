#pragma once
#include "SkeletonTypes.h"
#include "Kines.h"
#include "CoreMinimal.h"
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
		KeyToMesh = MakeShareable(new TMap<FSkeletonKey, FSMInstanceId>());
		MeshToKey = MakeShareable(new TMap<FSMInstanceId, FSkeletonKey>());
	}
	
	virtual TOptional<FTransform> GetTransformCopy(FSkeletonKey Target)
	{
		auto m = KeyToMesh->Find(Target);
		FTransform ref;
		if(m)
		{
			if (GetInstanceTransform(m->InstanceIndex, ref, true))
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
			return SetSMInstanceTransform(*m, Update, true, true, true);
		}
		return false;
	};
	virtual FSkeletonKey GetKeyOfInstance(FSMInstanceId Target)
	{
		auto m = MeshToKey->Find(Target);
		return m ? *m : FSkeletonKey();
	};

	virtual void AddToMap(FSMInstanceId MeshId, FSkeletonKey Key)
	{
		KeyToMesh->Add(Key, MeshId);
		MeshToKey->Add(MeshId, Key);
	}

	virtual void CleanupInstance(const FSkeletonKey Target)
	{
		auto m = KeyToMesh->Find(Target);
		if (m != nullptr)
		{
			MeshToKey->Remove(*m);
		}
		KeyToMesh->Remove(Target);
	}
	
private:
	TSharedPtr<TMap<FSkeletonKey, FSMInstanceId>> KeyToMesh;
	TSharedPtr<TMap<FSMInstanceId, FSkeletonKey>> MeshToKey;
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
