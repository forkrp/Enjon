#ifndef ENTITY_H
#define ENTITY_H

#include "System/Types.h"
#include "ECS/Components.h"
//#include "ComponentSystems.h"

// Attempt at doing Composition instead of Inheritance based objects

#define MAX_ENTITIES		250000  // Max number of possible entities allowed

namespace ECS { 

	using eid32 = Enjon::uint32;
	using bitmask32 = Enjon::uint32;

	enum ComponentMasks : bitmask32
	{
		COMPONENT_NONE					= 0x00000000,
		COMPONENT_LABEL					= 0x00000001,
		COMPONENT_TRANSFORM3D			= 0x00000002,
		COMPONENT_RENDERER2D			= 0x00000004,
		COMPONENT_ANIMATION2D			= 0x00000008, 
		COMPONENT_PLAYERCONTROLLER		= 0x00000010, 
		COMPONENT_AICONTROLLER			= 0x00000020, 
		COMPONENT_HEALTH 				= 0x00000040
	}; 

}


#endif