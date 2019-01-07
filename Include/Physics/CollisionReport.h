// @file CollisionReport.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#ifndef ENJON_COLLISION_REPORT_H
#define ENJON_COLLISION_REPORT_H

#include "Entity/EntityManager.h"

namespace Enjon
{
	class RigidBodyComponent;
	class CollisionReport
	{ 
		public:

			CollisionReport( ComponentHandle< RigidBodyComponent >* collider )
				: mCollider( collider )
			{
			}
 
			ComponentHandle< RigidBodyComponent >* mCollider;
	}; 
}

#endif
