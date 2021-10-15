#pragma once

#include <memory>

typedef unsigned int GLuint;
typedef int GLint;

namespace Engine
{

	class Texture
	{
	public:
		Texture(GLuint texture)
			: m_texture(texture)
		{
		}

		static void unbind(int slot);
		virtual ~Texture();

		void bind(int slot) const;

	protected:
		GLuint m_texture { 0 };

	};

}
