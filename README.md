# SkeletonKey
Skeleton key is a basal key "type" used across all libraries maintained by Breach Dogs. These keys must be generated in consistent ways, and are used across almost every ECS-like component we have made. Ultimately, it was the emergence of the need for an up-projection into the concept of an actor for the purposes of assigning abilities and attributes to non-actors that drove the creation of ObjectKey, and with it, the defactoring of the Key Types.

## Why Is This A Plugin?
While it's unfortunate that this ended up in a plugin, because that's sort of funny for one file, we need versioning, loading, and dependency assurances. It also means that some keys may move into the skeleton key plugin, which would honestly be nice for making the individual plug-ins less weird. In particular, we currently have BristleTime and ArtilleryTime, but it'd be nice to have just StepTime. Without a top level types lib, that's not really elegantly doable in UE+C++ without esoterica that I don't want to employ. 


## Mechanism
Skeleton keys contain an 8 bit typing prefix, with 0 being reserved for "not an object key." This can be used for runtime type inference without reflecting in constant time and one branch, allowing any key for a type of object to be transparently used as an objectkey that will route correctly through the ECS. This is not a very elegant solution, as it permits us only 255 types of objects, but if we exhaust that space, we probably deserve what happens next. The base input into a skeleton key must be a MSB\LSB neutral hash function with as close to a pure random distribution as possible. As a result, LSH cannot be used for skeleton keys, because we transparently mask the key prefix into the MSB
