#ifndef SPRITE_H
#define SPRITE_H 

#include <string>

#include <GLEW/glew.h>
#include "IO/ResourceManager.h"

#include "Graphics/GLTexture.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Enjon { namespace Graphics { 

	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void Init(float x, float y, float width, float height, std::string texturePath);

		void Draw();
		
	private:
		float m_x;
		float m_y;
		float m_width;
		float m_height;
		GLuint m_vbo;
		GLTexture m_texture;

	}; 
}}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
