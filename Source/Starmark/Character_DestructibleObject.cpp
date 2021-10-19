#include "Character_DestructibleObject.h"


ACharacter_DestructibleObject::ACharacter_DestructibleObject()
{
	// Disable unused components
	ActorSelectedPlane->Deactivate();
	ActorSelected->Deactivate();
}