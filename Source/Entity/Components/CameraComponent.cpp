#include "Entity/Components/CameraComponent.h"
#include "Entity/EntityManager.h" 
#include "Graphics/GraphicsScene.h"
#include "Serialize/AssetArchiver.h"
#include "Asset/AssetManager.h"
#include "Graphics/GraphicsSubsystem.h"
#include "SubsystemCatalog.h"
#include "Engine.h"

namespace Enjon
{
	//====================================================================

	void CameraComponent::ExplicitConstructor()
	{ 
		// Get graphics subsytem
		GraphicsSubsystem* gs = EngineSubsystem( GraphicsSubsystem );

		// Set up camera using viewport from graphics subsystem
		mCamera = Camera( gs->GetViewport( ) );

		// Add renderable to scene
		gs->GetGraphicsScene( )->AddCamera( &mCamera );

		// Set explicit tick state
		mTickState = ComponentTickState::TickAlways;
	} 

	//====================================================================

	void CameraComponent::ExplicitDestructor()
	{
		// Remove renderable from scene
		if ( mCamera.GetGraphicsScene() != nullptr )
		{
			mCamera.GetGraphicsScene( )->RemoveCamera( &mCamera );
		}
	} 

	//==================================================================== 

	void CameraComponent::Initialize( )
	{
		// Set camera to active camera in scene
		if ( mCamera.GetGraphicsScene( ) )
		{
			mCamera.GetGraphicsScene( )->SetActiveCamera( &mCamera );
		}
	}

	//==================================================================== 

	void CameraComponent::SetActiveCamera( )
	{
		if ( mCamera.GetGraphicsScene( ) )
		{
			mCamera.GetGraphicsScene( )->SetActiveCamera( &mCamera );
		}
	}

	//==================================================================== 

	void CameraComponent::Update( )
	{
		Transform wt = mEntity->GetWorldTransform( );
		mCamera.SetPosition( wt.GetPosition( ) ); 
		mCamera.SetRotation( wt.GetRotation().Normalize() );
	}
	
	//==================================================================== 

	GraphicsScene* CameraComponent::GetGraphicsScene() const
	{ 
		return mCamera.GetGraphicsScene(); 
	} 

	//====================================================================

	void CameraComponent::SetGraphicsScene(GraphicsScene* scene)
	{
		mCamera.SetGraphicsScene(scene);
	} 

	//==================================================================== 

	Camera* CameraComponent::GetCamera( )
	{
		return &mCamera;
	}

	//==================================================================== 

	Result CameraComponent::OnEditorUI( )
	{
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );
		igm->DebugDumpObject( &mCamera );
		return Result::SUCCESS;
	}

	//==================================================================== 
}
