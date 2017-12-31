#include "EditorApp.h"

#include <Engine.h>
#include <Asset/AssetManager.h>
#include <SubsystemCatalog.h>
#include <IO/InputManager.h>
#include <ImGui/ImGuiManager.h>
#include <Graphics/GraphicsSubsystem.h> 
#include <Graphics/Window.h>
#include <Entity/EntityManager.h>
#include <Entity/Components/GraphicsComponent.h>
#include <Utils/FileUtils.h>

#include <windows.h>
#include <fmt/format.h>
#include <chrono>
#include <ctime>

typedef void( *funcSetEngineInstance )( Enjon::Engine* instance );
typedef Enjon::Result( *funcStartUp )( const Enjon::f32& );
typedef Enjon::Result( *funcUpdate )( const Enjon::f32& );
typedef Enjon::Result( *funcShutDown )( const Enjon::f32& );
typedef Enjon::Result( *funcSerializeData )( Enjon::ByteBuffer* );
typedef Enjon::Result( *funcDeserializeData )( Enjon::ByteBuffer* );

// TODO(): Make sure to abstract this for platform independence
HINSTANCE dllHandleTemp = nullptr;
HINSTANCE dllHandle = nullptr;
funcSetEngineInstance setEngineFunc = nullptr;
funcStartUp startUpFunc = nullptr;
funcUpdate updateFunc = nullptr;
funcShutDown shutDownFunc = nullptr;
funcSerializeData serializeDataFunc = nullptr;
funcDeserializeData deserializeDataFunc = nullptr;

namespace fs = std::experimental::filesystem; 

Enjon::String copyDir = "E:/Development/C++DLLTest/Build/Debug/TestDLLIntermediate/";

void CopyTempDLL( )
{
	Enjon::String rootDir = Enjon::Engine::GetInstance( )->GetConfig( ).GetRoot( );

	fs::path dllPath = rootDir + "Build/Debug/TestDLLTemp.dll";
	if ( fs::exists( dllPath ) )
	{
		fs::remove( dllPath );
	}

	// Now copy over contents from intermediate build to executable dir
	dllPath = copyDir;
	if ( fs::exists( dllPath ) )
	{
		fs::copy( fs::path( dllPath.string( ) + "TestDLL.dll" ), rootDir + "Build/Debug/TestDLLTemp.dll" );
	}

}

void CopyLibraryContents( )
{
	Enjon::String rootDir = Enjon::Engine::GetInstance( )->GetConfig( ).GetRoot( );

	fs::path dllPath = rootDir + "Build/Debug/TestDLL.dll";
	if ( fs::exists( dllPath ) )
	{
		fs::remove( dllPath );
	}

	// Now copy over contents from intermediate build to executable dir
	dllPath = copyDir;
	if ( fs::exists( dllPath ) )
	{
		fs::copy( fs::path( dllPath.string( ) + "TestDLL.dll" ), rootDir + "Build/Debug/TestDLL.dll" );
	}
}

void SceneView( bool* viewBool )
{
	const Enjon::GraphicsSubsystem* gfx = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( );
	u32 currentTextureId = gfx->GetCurrentRenderTextureId( ); 

	// Render game in window
	ImVec2 cursorPos = ImGui::GetCursorScreenPos( );

	ImTextureID img = ( ImTextureID )currentTextureId;
	ImGui::Image( img, ImVec2( ImGui::GetWindowWidth( ), ImGui::GetWindowHeight( ) ),
		ImVec2( 0, 1 ), ImVec2( 1, 0 ), ImColor( 255, 255, 255, 255 ), ImColor( 255, 255, 255, 0 ) );

	ImVec2 min = ImVec2( cursorPos.x + ImGui::GetContentRegionAvailWidth( ) - 100.0f, cursorPos.y + 10.0f );
	ImVec2 max = ImVec2( min.x + 50.0f, min.y + 10.0f );

	ImGui::SetCursorScreenPos( min );
	auto drawlist = ImGui::GetWindowDrawList( );
	f32 fps = ImGui::GetIO( ).Framerate;

	// Update camera aspect ratio
	gfx->GetSceneCamera( )->ConstCast< Enjon::Camera >( )->SetAspectRatio( ImGui::GetWindowWidth( ) / ImGui::GetWindowHeight( ) );
}

void EnjonEditor::CameraOptions( bool* enable )
{
	const Enjon::GraphicsSubsystem* gfx = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( ); 
	const Enjon::Camera* cam = gfx->GetSceneCamera( );

	if ( ImGui::TreeNode( "Camera" ) )
	{
		Enjon::ImGuiManager::DebugDumpObject( cam ); 
		ImGui::TreePop( );
	}

	ImGui::DragFloat( "Camera Speed", &mCameraSpeed, 0.1f ); 
}

void EnjonEditor::PlayOptions( )
{
	if ( mPlaying )
	{
		if ( ImGui::Button( "Stop" ) )
		{ 
			mPlaying = false; 

			// Call shut down function for game
			if ( shutDownFunc )
			{
				shutDownFunc( 0.01f );
			}

			auto cam = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( )->ConstCast< Enjon::GraphicsSubsystem >( )->GetSceneCamera( )->ConstCast< Enjon::Camera >();
			cam->SetPosition( mPreviousCameraTransform.Position );
			cam->SetRotation( mPreviousCameraTransform.Rotation ); 
		}
	}
	else
	{
		if ( ImGui::Button( "Play" ) )
		{ 
			mPlaying = true;

			auto cam = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( )->ConstCast< Enjon::GraphicsSubsystem >( )->GetSceneCamera( ); 
			mPreviousCameraTransform = Enjon::Transform( cam->GetPosition(), cam->GetRotation(), Enjon::Vec3( cam->GetOrthographicScale() ) );

			// Call start up function for game
			if ( startUpFunc )
			{
				startUpFunc( 0.01f );
			}
			else
			{ 
				std::cout << "Cannot play without game loaded!\n";
				mPlaying = false;
			}
		}
	} 
}

void EnjonEditor::WorldOutlinerView( )
{
	const Enjon::EntityManager* entities = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::EntityManager >( );
	ImGui::Text( fmt::format( "Entities: {}", entities->GetActiveEntities().size() ).c_str() ); 

	for ( auto& e : entities->GetActiveEntities( ) )
	{
		if ( ImGui::TreeNode( fmt::format( "{}", e->GetID( ) ).c_str( ) ) )
		{
			Enjon::ImGuiManager::DebugDumpObject( e );

			if ( ImGui::TreeNode( fmt::format( "Components##{}", e->GetID( ) ).c_str( ) ) )
			{
				for ( auto& c : e->GetComponents( ) )
				{
					Enjon::ImGuiManager::DebugDumpObject( c );
				} 
				ImGui::TreePop( );
			}

			ImGui::TreePop( );
		}
	}
}

void EnjonEditor::LoadResourceFromFile( )
{
	// Load file
	if ( ImGui::CollapsingHeader( "Load Resource" ) )
	{
		char buffer[ 256 ];
		std::strncpy( buffer, mResourceFilePathToLoad.c_str( ), 256 );
		if ( ImGui::InputText( "File Path", buffer, 256 ) )
		{
			mResourceFilePathToLoad = Enjon::String( buffer );
		}

		if ( ImGui::Button( "Load File" ) )
		{
			Enjon::AssetManager* am = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::AssetManager >( )->ConstCast< Enjon::AssetManager >( );
			am->AddToDatabase( mResourceFilePathToLoad );
		}
	}
}
 
Enjon::Result EnjonEditor::Initialize( )
{ 
	mApplicationName = "EnjonEditor";

	Enjon::String mAssetsDirectoryPath = Enjon::Engine::GetInstance()->GetConfig().GetRoot() + "Editor/Assets/";
	Enjon::String cacheDirectoryPath = mAssetsDirectoryPath + "/Cache/";

	// Get asset manager and set its properties ( I don't like this )
	Enjon::AssetManager* mAssetManager = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get<Enjon::AssetManager>( )->ConstCast< Enjon::AssetManager >( );
	Enjon::GraphicsSubsystem* mGfx = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( )->ConstCast< Enjon::GraphicsSubsystem >( );

	// This also needs to be done through a config file or cmake
	mAssetManager->SetAssetsDirectoryPath( mAssetsDirectoryPath );
	mAssetManager->SetCachedAssetsDirectoryPath( cacheDirectoryPath );
	mAssetManager->SetDatabaseName( GetApplicationName( ) );
	mAssetManager->Initialize( ); 

	// Register individual windows
	Enjon::ImGuiManager::RegisterWindow( [ & ] ( )
	{
		// Docking windows
		if ( ImGui::BeginDock( "Scene", &mViewBool, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
		{
			// Print docking information
			SceneView( &mViewBool );
		}
		ImGui::EndDock( ); 
	}); 

	Enjon::ImGuiManager::RegisterWindow( [ & ]
	{
		if ( ImGui::BeginDock( "Camera", &mShowCameraOptions ) )
		{
			CameraOptions( &mShowCameraOptions );
		}
		ImGui::EndDock( );
	});

	Enjon::ImGuiManager::RegisterWindow( [ & ]
	{
		if ( ImGui::BeginDock( "Load Resource", &mShowLoadResourceOption ) )
		{
			LoadResourceFromFile( );
		}
		ImGui::EndDock( );
	});

	Enjon::ImGuiManager::RegisterWindow( [ & ]
	{
		if ( ImGui::BeginDock( "World Outliner", nullptr ) )
		{
			WorldOutlinerView( );
		}
		ImGui::EndDock( );
	});

	Enjon::ImGuiManager::RegisterWindow( [ & ]
	{
		if ( ImGui::BeginDock( "Play Options", nullptr ) )
		{
			PlayOptions( );
		}
		ImGui::EndDock( );
	} );

	// Register docking layouts
	Enjon::ImGuiManager::RegisterDockingLayout( ImGui::DockingLayout( "Scene", nullptr, ImGui::DockSlotType::Slot_Top, 1.0f ) );
	Enjon::ImGuiManager::RegisterDockingLayout( ImGui::DockingLayout( "Camera", "Scene", ImGui::DockSlotType::Slot_Right, 0.2f ) );
	Enjon::ImGuiManager::RegisterDockingLayout( ImGui::DockingLayout( "Load Resource", "Camera", ImGui::DockSlotType::Slot_Bottom, 0.3f ) );
	Enjon::ImGuiManager::RegisterDockingLayout( ImGui::DockingLayout( "World Outliner", "Camera", ImGui::DockSlotType::Slot_Top, 0.7f ) ); 
	Enjon::ImGuiManager::RegisterDockingLayout( ImGui::DockingLayout( "Play Options", "Scene", ImGui::DockSlotType::Slot_Top, 0.07f ) );

	return Enjon::Result::SUCCESS;
}

Enjon::Result EnjonEditor::Update( f32 dt )
{ 
	static float t = 0.0f;
	t += dt; 

	// Simulate game tick scenario if playing
	if ( mPlaying )
	{ 
		if ( updateFunc )
		{
			updateFunc( dt );
		} 
	}

	return Enjon::Result::PROCESS_RUNNING;
}

Enjon::Result EnjonEditor::ProcessInput( f32 dt )
{ 
	const Enjon::GraphicsSubsystem* mGfx = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem>( );
	const Enjon::Input* mInput = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::Input >( );
	Enjon::Camera* camera = mGfx->GetSceneCamera( )->ConstCast< Enjon::Camera >(); 

	if ( mInput->IsKeyPressed( Enjon::KeyCode::T ) )
	{
		mMoveCamera ^= 1;
	}

	if ( mMoveCamera )
	{
		Enjon::Vec3 velDir( 0, 0, 0 );

		if ( mInput->IsKeyDown( Enjon::KeyCode::W ) )
		{
			Enjon::Vec3 F = camera->Forward( );
			velDir += F;
		}
		if ( mInput->IsKeyDown( Enjon::KeyCode::S ) )
		{
			Enjon::Vec3 B = camera->Backward( );
			velDir += B;
		}
		if ( mInput->IsKeyDown( Enjon::KeyCode::A ) )
		{
			velDir += camera->Left( );
		}
		if ( mInput->IsKeyDown( Enjon::KeyCode::D ) )
		{
			velDir += camera->Right( );
		}

		// Normalize velocity
		velDir = Enjon::Vec3::Normalize( velDir );

		// Set camera position
		camera->Transform.Position += dt * mCameraSpeed * velDir;

		// Set camera rotation
		// Get mouse input and change orientation of camera
		Enjon::Vec2 mouseCoords = mInput->GetMouseCoords( );

		Enjon::iVec2 viewPort = mGfx->GetViewport( );

		f32 mouseSensitivity = 10.0f;

		// Grab window from graphics subsystem
		Enjon::Window* window = mGfx->GetWindow( )->ConstCast< Enjon::Window >( );

		// Set cursor to not visible
		window->ShowMouseCursor( false );

		// Reset the mouse coords after having gotten the mouse coordinates
		SDL_WarpMouseInWindow( window->GetWindowContext( ), ( f32 )viewPort.x / 2.0f, ( f32 )viewPort.y / 2.0f );

		// Offset camera orientation
		f32 xOffset = Enjon::ToRadians( ( f32 )viewPort.x / 2.0f - mouseCoords.x ) * dt * mouseSensitivity;
		f32 yOffset = Enjon::ToRadians( ( f32 )viewPort.y / 2.0f - mouseCoords.y ) * dt * mouseSensitivity;
		camera->OffsetOrientation( xOffset, yOffset );
	}
	else
	{
		mGfx->GetWindow( )->ConstCast< Enjon::Window >( )->ShowMouseCursor( true );
	}

	// DLL Loading/Unloading
	{
		if ( mInput->IsKeyPressed( Enjon::KeyCode::R ) )
		{
			Enjon::ByteBuffer buffer;
			bool needsReload = false;

			// Serialize state for reload and free library
			if ( dllHandle )
			{ 
				if ( mPlaying )
				{
					if ( shutDownFunc )
					{ 
						// NOTE(): Will not work if the layout of the data has changed! Need versioning working first...
						if ( serializeDataFunc )
						{
							serializeDataFunc( &buffer );
							needsReload = true; 
						} 

						// Destroy the instance of the original handle
						shutDownFunc( dt );
					} 
				}

				// Free library if in use
				FreeLibrary( dllHandle );
				dllHandle = nullptr;
				startUpFunc = nullptr;
				updateFunc = nullptr;
				shutDownFunc = nullptr;
				serializeDataFunc = nullptr;
				deserializeDataFunc = nullptr;
			}

			// Copy files to directory
			CopyLibraryContents( );

			// Try to load library
			dllHandle = LoadLibrary( "TestDLL.dll" );

			// If valid, then set address of procedures to be called
			if ( dllHandle )
			{
				// Set functions from handle
				startUpFunc = ( funcStartUp )GetProcAddress( dllHandle, "StartUp" );
				updateFunc = ( funcUpdate )GetProcAddress( dllHandle, "Update" );
				shutDownFunc = ( funcShutDown )GetProcAddress( dllHandle, "ShutDown" );
				serializeDataFunc = ( funcSerializeData )GetProcAddress( dllHandle, "SerializeData" );
				deserializeDataFunc = ( funcDeserializeData )GetProcAddress( dllHandle, "DeserializeData" );

				// Try and set the engine instance
				setEngineFunc = ( funcSetEngineInstance )GetProcAddress( dllHandle, "SetEngineInstance" );
				if ( setEngineFunc )
				{
					setEngineFunc( Enjon::Engine::GetInstance( ) );
				}

				if ( needsReload )
				{
					if ( deserializeDataFunc )
					{
						deserializeDataFunc( &buffer );
					}
				}

			}
			else
			{
				std::cout << "Could not load library\n";
			}
		}
	}

	// Starting /Stopping game instance
	{
		if ( mInput->IsKeyPressed( Enjon::KeyCode::P ) )
		{
			if ( !mPlaying )
			{
				mPlaying = true;

				auto cam = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( )->ConstCast< Enjon::GraphicsSubsystem >( )->GetSceneCamera( );
				mPreviousCameraTransform = Enjon::Transform( cam->GetPosition(), cam->GetRotation(), Enjon::Vec3( cam->GetOrthographicScale() ) );

				// Call start up function for game
				if ( startUpFunc )
				{
					startUpFunc( dt );
				}
				else
				{
					std::cout << "Cannot play without game loaded!\n";
					mPlaying = false;
				}
			}
		}

		if ( mInput->IsKeyPressed( Enjon::KeyCode::Escape ) )
		{
			if ( mPlaying )
			{
				mPlaying = false; 

				// Call shut down function for game
				if ( shutDownFunc )
				{
					shutDownFunc( dt );
				}

				auto cam = Enjon::Engine::GetInstance( )->GetSubsystemCatalog( )->Get< Enjon::GraphicsSubsystem >( )->ConstCast< Enjon::GraphicsSubsystem >( )->GetSceneCamera( )->ConstCast< Enjon::Camera >( );
				cam->SetPosition( mPreviousCameraTransform.Position );
				cam->SetRotation( mPreviousCameraTransform.Rotation );
			}
		}
	}

	return Enjon::Result::PROCESS_RUNNING;
}

Enjon::Result EnjonEditor::Shutdown( )
{ 
	return Enjon::Result::SUCCESS;
}
