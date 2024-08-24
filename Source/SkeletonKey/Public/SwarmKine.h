﻿#pragma once
#include "SkeletonTypes.h"
#include "Kines.h"
#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"

class UObject;

//interface that adds support for owning swarm kines. used for managing many many meshes at a time.
//generally, 
UCLASS()
class SKELETONKEY_API UAUKineManager : public UInstancedStaticMeshComponent
{
public:
	explicit UAUKineManager()
		: KeyToMesh(), MeshToKey()
	{
	}
	virtual TOptional<FTransform> GetTransformCopy(ObjectKey Target)
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
	virtual bool SetTransformOnInstance(ObjectKey Target, FTransform Update)
	{
		auto m = KeyToMesh->Find(Target);
		if(m)
		{
			return SetSMInstanceTransform(*m, Update, true, true, true);
		}
		return  false;
	};
	virtual ObjectKey GetKeyOfInstance(FSMInstanceId Target)
	{
		auto m = MeshToKey->Find(Target);
		return m ? *m : ObjectKey();
	};
	
private:
	TSharedPtr<TMap<ObjectKey, FSMInstanceId>> KeyToMesh;
	TSharedPtr<TMap<FSMInstanceId, ObjectKey>> MeshToKey;
};


class SwarmKine : public Kine
{

	TWeakObjectPtr<UAUKineManager> MyManager;

	virtual TOptional<FTransform> CopyOfTransformlike() override
	{
		return MyManager->GetTransformCopy(MyKey);
	}

	virtual void SetTransformlike(FTransform Input) override
	{
		MyManager->SetTransformOnInstance(MyKey, Input);
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
};