// @file SkeletalAnimation.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_SKELETAL_ANIMATION_H
#define ENJON_SKELETAL_ANIMATION_H

#include "Base/Object.h"
#include "System/Types.h"
#include "Defines.h"
#include "Math/Maths.h"
#include "Asset/Asset.h"

namespace Enjon
{
	// Forward Declarations
	class Skeleton;
	class SkeletalAnimation;
	class SkeletalAnimationAssetLoader;
	class ChannelData;

	template <typename T>
	class KeyFrame
	{ 
		friend ChannelData;
		friend SkeletalAnimation;
		friend SkeletalAnimationAssetLoader;
		friend Skeleton;

		public:

			/*
			* @brief
			*/
			KeyFrame( ) = default;

			/*
			* @brief
			*/
			~KeyFrame( ) = default;

			/*
			* @brief
			*/
			KeyFrame( const f32& timeStamp, const T& value )
				: mTimeStamp( timeStamp ), mValue( value )
			{ 
			} 

		protected:

		private: 
			T mValue;
			f32 mTimeStamp;
	};

	struct ChannelData
	{
		u32 GetRotationFrameID( const f32& time );
		u32 GetPositionFrameID( const f32& time );
		u32 GetScaleFrameID( const f32& time );

		Vector< KeyFrame< Quaternion > > mRotationKeys;
		Vector< KeyFrame< Vec3 > > mPositionKeys;
		Vector< KeyFrame< Vec3 > > mScaleKeys;
	};

	ENJON_CLASS( Construct )
	class SkeletalAnimation : public Asset
	{ 
		friend SkeletalAnimationAssetLoader;
		friend Skeleton;

		ENJON_CLASS_BODY( )

		public:

			/*
			* @brief
			*/
			SkeletalAnimation( );

			/*
			* @brief
			*/
			~SkeletalAnimation( ); 

			/*
			* @brief
			*/
			Transform CalculateInterpolatedTransform( const f32& time, const u32& boneID );

			/*
			* @brief
			*/
			f32 GetDuration( ) const;

		protected: 

		private:
			Vector< ChannelData > mChannelData;
			f32 mTicksPerSecond = 30.0f;
			f32 mNumberOfTicks = 1.0f;
	};
}

#endif
