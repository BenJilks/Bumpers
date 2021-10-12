#pragma once

#include <memory>

typedef unsigned int GLuint;

namespace Engine
{

	class Texture
	{
	public:
		static void unbind();
		virtual ~Texture();

		void bind(int slot) const;

	protected:
		Texture(GLuint texture)
			: m_texture(texture)
		{
		}

		GLuint m_texture { 0 };

	};

}
