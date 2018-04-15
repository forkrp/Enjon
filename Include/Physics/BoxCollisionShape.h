// @file BoxCollisionShape.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_BOXCOLLISIONSHAPE_H
#define ENJON_BOXCOLLISIONSHAPE_H

#include "Physics/CollisionShape.h"
#include "Math/Vec3.h"

namespace Enjon
{
	class Vec3;

	ENJON_CLASS( Construct )
	class BoxCollisionShape : public CollisionShape
	{
		ENJON_CLASS_BODY( )

		public:

			/**
			* @brief
			*/
			BoxCollisionShape( );

			/**
			* @brief
			*/
			BoxCollisionShape( RigidBody* body );

			/**
			* @brief
			*/
			~BoxCollisionShape( );

			/*
			* @brief
			*/
			ENJON_FUNCTION( )
			void SetHalfExtents( const Vec3& extents );

		private: 

			/**
			* @brief
			*/
			virtual void Base( ) override; 

			/**
			* @brief
			*/
			void ConstructShape( ); 

		private:

			ENJON_PROPERTY( UIMin = 0, UIMax = 1, Delegates[ Mutator = SetHalfExtents ] )
			Vec3 mHalfExtents = Vec3( 1.0f ); 
	};
}

#endif
