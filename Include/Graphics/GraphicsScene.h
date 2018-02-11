#pragma once
#ifndef ENJON_GRAPHICS_SCENE_H
#define ENJON_GRAPHICS_SCENE_H

#include "Defines.h"
#include "System/Types.h"
#include "Graphics/Color.h"
#include "Base/Object.h"

#include <set>
#include <vector>

namespace Enjon { 

	class Renderable;
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class QuadBatch;

	enum class RenderableSortType
	{
		MATERIAL,
		DEPTH,
		NONE
	};

	struct AmbientSettings
	{
		AmbientSettings()
		{
			mColor = RGBA32_White();
			mIntensity = 1.0f;
		}

		AmbientSettings(ColorRGBA32& color, float intensity)
			: mColor(color), mIntensity(intensity)
		{
		}

		ColorRGBA32 mColor;	
		float mIntensity;
	};

	using RenderableID = u32;

	ENJON_CLASS( )
	class GraphicsScene : public Enjon::Object
	{
		ENJON_CLASS_BODY( )

		public:
			/*
			* @brief
			*/
			GraphicsScene();

			/*
			* @brief
			*/
			~GraphicsScene();

			/*
			* @brief
			*/
			void AddRenderable(Renderable* renderable);

			/*
			* @brief
			*/
			void AddNonDepthTestedRenderable(Renderable* renderable);

			/*
			* @brief
			*/
			void RemoveRenderable(Renderable* renderable);

			/*
			* @brief
			*/
			void RemoveNonDepthTestedRenderable(Renderable* renderable);

			/*
			* @brief
			*/
			void AddQuadBatch(QuadBatch* batch);

			/*
			* @brief
			*/
			void RemoveQuadBatch(QuadBatch* batch);

			/*
			* @brief
			*/
			void AddDirectionalLight(DirectionalLight* light);
			
			/*
			* @brief
			*/
			void RemoveDirectionLight(DirectionalLight* light);

			/*
			* @brief
			*/
			void AddPointLight(PointLight* light);

			/*
			* @brief
			*/
			void RemovePointLight(PointLight* light);

			/*
			* @brief
			*/
			void AddSpotLight(SpotLight* light);

			/*
			* @brief
			*/
			void RemoveSpotLight(SpotLight* light);

			/*
			* @brief
			*/
			AmbientSettings* GetAmbientSettings() { return &mAmbientSettings; }

			/*
			* @brief
			*/
			void SetAmbientSettings(AmbientSettings& settings);

			/*
			* @brief
			*/
			void SetAmbientColor(ColorRGBA32& color);

			/*
			* @brief
			*/
			void SetSun(DirectionalLight* light) { mSun = light; }
			
			/*
			* @brief
			*/
			DirectionalLight* GetSun() const { return mSun; }

			/*
			* @brief
			*/
			const std::vector<Renderable*>& GetRenderables() const;

			/*
			* @brief
			*/
			const std::vector<Renderable*>& GetNonDepthTestedRenderables();

			/*
			* @brief
			*/
			const std::set<QuadBatch*>& GetQuadBatches() const { return mQuadBatches; }

			/*
			* @brief
			*/
			const std::set<DirectionalLight*>& GetDirectionalLights() const { return mDirectionalLights; }

			/*
			* @brief
			*/
			const std::set<PointLight*>& GetPointLights() const { return mPointLights; }

			/*
			* @brief
			*/
			const std::set<SpotLight*>& GetSpotLights() const { return mSpotLights; }

		private:
			
			/*
			* @brief
			*/
			void AssignRenderableID(Renderable* renderable);

			/*
			* @brief
			*/
			void SortRenderables( RenderableSortType type = RenderableSortType::DEPTH );

		private:

			/*
			* @brief
			*/
			static bool CompareMaterial(Renderable* a, Renderable* b);

			/*
			* @brief
			*/
			static bool CompareDepth(Renderable* a, Renderable* b);

		private:

			ENJON_PROPERTY( )
			Vector< Renderable* > mSortedRenderables; 
 
			Vector<Renderable*> mNonDepthTestedRenderables;

			std::set<Renderable*> mRenderables;
			std::set<QuadBatch*> mQuadBatches;
			std::set<DirectionalLight*> mDirectionalLights;
			std::set<PointLight*> mPointLights;
			std::set<SpotLight*> mSpotLights; 
			AmbientSettings mAmbientSettings; 
			DirectionalLight* mSun = nullptr; 

			// Scene should own all the data for renderables, lights, etc. and should hand those out on request from components
			// Or could have a graphics subsystem which holds all of these objects?
			/*
				template <typename T, typename... Args>
				const T* GraphicsFactory::Construct( Args&&... args )
				{ 
				}

				// Should scene own its own camera? How will this work, exactly?

				// Should scene actually be an asset in and of itself? So the graphics subsystem will actually hold an asset handle to a scene as opposed to a scene itself? Not sure that I really like that approach though, since it
					seems fairly abstracted away...

				Result Scene::SerializeData( ByteBuffer* buffer )
				{ 
					// Camera data
					ObjectArchiver::Serialize( &mCamera, buffer );

					// Get the entity manager
					const EntityManager* em = Engine::GetInstance()->GetSubsystemCatalog()->Get< EntityManager >();	

					buffer->Write< u32 >( em->GetActiveEntities().size() );

					// Write all entity data to buffer
					for ( auto& e : em->GetActiveEntities() )
					{
						// Only write out top-level entities
						if ( !e.HasParent() )
						{
							EntityArchiver::Serialize( e, buffer );
						} 
					}
				}

				Result DeserializeData( ByteBuffer* buffer )
				{ 
					// Deserialize camera data
					ObjectArchiver::Deserialize( buffer, &mCam );

					// Deserialize entity data 
					u32 entityCount = buffer->Read< u32 >();

					for ( u32 i = 0; i < entityCount; ++i )
					{
						EntityArchiver::Deserialize( buffer );
					} 
				}
			*/ 
	};
}

#endif