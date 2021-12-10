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

		virtual void bind(int slot) const;

        inline bool has_loaded() const { return m_has_loaded; }

	protected:
		GLuint m_texture { 0 };
        bool m_has_loaded { false };

	};

}

