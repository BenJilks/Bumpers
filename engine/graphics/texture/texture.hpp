#pragma once

#include <memory>

#ifdef WIN32
typedef void* HANDLE;
#else
#include <mutex>
#endif

typedef unsigned int GLuint;
typedef int GLint;

namespace Engine
{

	class Texture
	{
	public:
		Texture(GLuint texture);

		static void unbind(int slot);
		virtual ~Texture();

		virtual void bind(int slot) const;
		bool has_loaded() const;

	protected:
		GLuint m_texture;

		mutable bool m_has_loaded { false };
		bool m_has_data_loaded { false };

#ifdef WIN32
		HANDLE m_loader_thread_mutex { nullptr };
#else
		mutable std::mutex m_loader_thread_mutex;
#endif

	};

}

