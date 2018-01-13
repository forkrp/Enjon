// @file RigidBody.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_RIGIDBODY_H
#define ENJON_RIGIDBODY_H

#include "Base/Object.h"
#include "System/Types.h"
#include "Math/Maths.h"
#include "Physics/CollisionShape.h"
#include "Physics/PhysicsDefines.h" 

namespace Enjon
{ 
	ENJON_CLASS( )
	class RigidBody : public Object
	{
		ENJON_CLASS_BODY( )

		public:

			/**
			* @brief
			*/
			RigidBody( );

			/**
			* @brief
			*/
			~RigidBody( );

			/**
			* @brief
			*/
			void Initialize( );

			/**
			* @brief
			*/
			void Reinitialize( );

			/**
			* @brief
			*/
			void AddToWorld( );

			/**
			* @brief
			*/
			void RemoveFromWorld( );

			/**
			* @brief
			*/
			void SetShape( CollisionShapeType type );

			/**
			* @brief
			*/
			void SetMass( const f32& mass );

			/**
			* @brief
			*/
			f32 GetMass( ) const;

			/**
			* @brief
			*/
			void SetRestitution( const f32& restitution );

			/**
			* @brief
			*/
			void SetLinearDamping( const f32& damping );

			/**
			* @brief
			*/
			void SetAngularDamping( const f32& damping );

			/**
			* @brief
			*/
			void SetFriction( const f32& friction );

			/**
			* @brief
			*/
			void SetGravity( const Vec3& gravity );

			/**
			* @brief
			*/
			void SetLinearVelocity( const Vec3& gravity );

			/**
			* @brief
			*/
			void ClearForces( );

			/**
			* @brief
			*/
			void ForceAwake( );

			/**
			* @brief
			*/
			void ApplyCentralForce( const Vec3& force );

			/**
			* @brief
			*/
			void ApplyRelativeForce( const Vec3& force, const Vec3& relativePosition );

			/**
			* @brief
			*/
			void ApplyCentralImpulse( const Vec3& force );

			/**
			* @brief
			*/
			void ApplyImpulse( const Vec3& force, const Vec3& relativeImpulse ); 

			/**
			* @brief
			*/
			BulletRigidBody* GetRawBody( ) const;

			/**
			* @brief
			*/
			BulletRigidBodyMotionState* RigidBody::GetMotionState( ) const; 

			/**
			* @brief
			*/
			CollisionShape* GetCollisionShape( ) const;

			/**
			* @brief
			*/
			void SetWorldTransform( const Transform& transform );

			/**
			* @brief
			*/
			Transform GetWorldTransform( );

			/**
			* @brief
			*/
			void SetLocalScaling( const Vec3& scale );

			/**
			* @brief
			*/
			void SetContinuousCollisionDetectionEnabled( bool enabled );

			/**
			* @brief
			*/
			void SetUserPointer( void* pointer );

		// Serialized properties
		private:

			/*

				Want to have a way to be able to register callback function for a metaproperty that will be called whenever it's changed...
				Could this only be editor specific, or game related as well? Not sure...
				How would this even look?

				ENJON_PROPERTY( OnChangedCallbackEnabled )
				f32 mFloatProperty = 10.0f; 

				These should be generated by default and compiled out on release builds...  

				Simple use case is that in the editor I change the value of the mass on the rigid body using a slider - This needs to actually call "SetMass" on 
				the RigidBodyComponent itself instead of directly calling it... So these callbacks could be registered to the class itself, I suppose?
				The function being set for the callback must be a reflected function...I think

				ENJON_FUNCTION( )
				void SetMass(const f32& mass );
 
				// Could associate the property with multiple callbacks?
				ENJON_PROPERTY( Editable, OnChangedCallback = ["SetMass", "SetSomethingElse..."] )
				f32 mFloatProperty = 1.0f;

				// So what does the signal look like?

				if ( ImGui::DragFloat(...) )
				{
					cls->SetValue( obj, prop, val );
				}
				
				==>
				
				// Set the properties and then makes callback
				template < typename T >
				void SetValue( const Object* object, const MetaProperty* prop, const T& value ) const
				{
					if ( HasProperty( prop ) )
					{
						T* dest = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
						*dest = value;

						// Do callback with prop
						prop->DoCallbacks();
					}
				} 


				// So what does THAT look like?
				Well, on generation of the reflection code, need to make sure that callback functions are generated as well


				// Or maybe I'll just have virtual setters/getters for the properties than can be used in editor code?
				if ( prop->HasSetter() )
				{
					cls->CallSetter( obj, prop, val );
				}

				prop->CallSetter(obj, val); 


			*/

			ENJON_PROPERTY( )
			f32 mMass = 1.0f;

			ENJON_PROPERTY( )
			f32 mRestitution = 0.0f;

			ENJON_PROPERTY( )
			f32 mFriction = 0.8f;

			ENJON_PROPERTY( )
			f32 mLinearDamping = 0.05f;

			ENJON_PROPERTY( )
			f32 mAngularDamping = 0.4f;

			ENJON_PROPERTY( )
			Vec3 mGravity = Vec3( 0.0f, -10.0f, 10.0f );

			ENJON_PROPERTY( )
			u32 mCCDEnabled = false;

			ENJON_PROPERTY( )
			CollisionShape* mShape = nullptr;

		private:
			BulletRigidBody* mBody = nullptr;
			BulletRigidBodyMotionState* mMotionState = nullptr;
			BulletDynamicPhysicsWorld* mWorld = nullptr; 
	};
}

#endif
