﻿#pragma once

#include "CoreMinimal.h"
#include "skeletonize.h"
#include "Containers/CircularQueue.h"
using BristleTime = long; //this will become uint32. don't bitbash this.
using ArtilleryTime = BristleTime;

//OBJECT KEY DOES NOT SKELETONIZE AUTOMATICALLY. OTHER KEY TYPES MUST DO THAT.
class ObjectKey
{
	friend class ActorKey;
public:
	uint64_t Obj;
	explicit ObjectKey()
	{
		//THIS WILL REGISTER AS NOT AN OBJECT KEY PER SKELETONIZE (SFIX_NONE)
		Obj = 0;
	}
	explicit ObjectKey(uint64 ObjIn)
	{
		Obj=ObjIn;
	}
	operator uint64() const {return Obj;};
	operator ActorKey() const;
	friend uint32 GetTypeHash(const ObjectKey& Other)
	{
		//while it looks like typehash can return uint64, it's undocumented and doesn't appear to work right.
		return GetTypeHash(Other.Obj);
	}
	ObjectKey& operator=(const ObjectKey& rhs) {
		if (this != &rhs) {
			Obj = rhs.Obj;
		}
		return *this;
	}

	//todo: add templated From method once this is settled enough.
	
	ObjectKey& operator=(const ActorKey& rhs);

	ObjectKey& operator=(const uint64 rhs) {
		Obj = rhs;
		return *this;
	}
};


static bool operator<(ObjectKey const& lhs, ObjectKey const& rhs) {
	return (lhs.Obj < rhs.Obj);
}

static bool operator==(ObjectKey const& lhs, ObjectKey const& rhs) {
	return (lhs.Obj == rhs.Obj);
}


class ActorKey
{
	friend class ObjectKey;
public:
	uint64_t Obj;
	explicit ActorKey()
	{
		//THIS FAILS THE OBJECT CHECK AND THE ACTOR CHECK. THIS IS INTENDED. THIS IS THE PURPOSE OF SKELETON KEY.
		Obj=0;
	}
	
	explicit ActorKey(const unsigned int rhs) {
		Obj = rhs;
		Obj <<= 32;
		//this doesn't seem like it should work, but because the SFIX bit patterns are intentionally asym
		//we actually do reclaim a bit of randomness.
		Obj += rhs; 
		Obj = FORGE_SKELETON_KEY(Obj, SKELLY::SFIX_ART_ACTS);
	}
	explicit ActorKey(uint64 ObjIn)
	{
		Obj=FORGE_SKELETON_KEY(ObjIn, SKELLY::SFIX_ART_ACTS);
	}
	operator uint64() const {return Obj;};
	operator ObjectKey() const {return ObjectKey(Obj);};
	friend uint32 GetTypeHash(const ActorKey& Other)
	{
		//it looks like get type hash can be a 64bit return? 
		return GetTypeHash(Other.Obj);
	}
	ActorKey& operator=(const uint64 rhs) {
		//should be idempotent.
		Obj = FORGE_SKELETON_KEY(rhs, SKELLY::SFIX_ART_ACTS);
		return *this;
	}
	ActorKey& operator=(const uint32 rhs) {
		//should be idempotent.
		Obj = rhs;
		Obj <<= 32;
		//this doesn't seem like it should work, but because the SFIX bit patterns are intentionally asym
		//we actually do reclaim a bit of randomness.
		Obj |= rhs; 
		Obj = FORGE_SKELETON_KEY(Obj, SKELLY::SFIX_ART_ACTS);
		return *this;
	}
	ActorKey& operator=(const ActorKey& rhs) {
		//should be idempotent.
		if (this != &rhs) {
			Obj = FORGE_SKELETON_KEY(rhs.Obj, SKELLY::SFIX_ART_ACTS);
		}
		return *this;
	}
	ActorKey& operator=(const ObjectKey& rhs) {
		//should be idempotent.
		Obj = FORGE_SKELETON_KEY(rhs.Obj, SKELLY::SFIX_ART_ACTS);
		return *this;
	}
};
static bool operator<(ActorKey const& lhs, ObjectKey const& rhs) {
	return (lhs.Obj < rhs.Obj);
}

inline ObjectKey::operator ActorKey() const
{return ActorKey(Obj);}

//FOR LEGACY REASONS, this applies the skeletonization.
inline ObjectKey& ObjectKey::operator=(const ActorKey& rhs)
{
	Obj = FORGE_SKELETON_KEY(rhs.Obj, SKELLY::SFIX_ART_ACTS);
	return *this;
}

static bool operator==(ActorKey const& lhs, ActorKey const& rhs) {
	return (lhs.Obj == rhs.Obj);
}

struct TransformUpdate
{
	ObjectKey ObjectKey;
	uint64 sequence;
	FQuat4f Rotation;// this alignment looks wrong. Like outright wrong.
	FVector3f Position;
	uint32 speed;// unused at the moment, here to support smearing if needed.
};

using TransformUpdatesForGameThread = TCircularQueue<TransformUpdate>;







