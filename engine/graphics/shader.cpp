// Includes
#include "shader.hpp"
#include "particals/partical_driver.hpp"
#include "gameobject/particals/partical_system.hpp"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <functional>
#include <cassert>
#include <glm/gtx/string_cast.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

static std::map<GLuint, std::string> load_source(const std::string &file_path)
{
	std::ifstream stream(file_path);
	if (!stream.good())
	{
		std::cerr << "Error opening file '" << file_path << "': " << strerror(errno) << "\n";
		return {};
	}

	std::string line, current_source;
	std::map<GLuint, std::string> source_map;
	GLuint current_shader = 0;

	while (std::getline(stream, line))
	{
		if (line.rfind("#shader ", 0) == 0)
		{
			// Flush the current shader 
			if (current_shader != 0) 
				source_map[current_shader] = current_source;
			current_source.clear();
			current_shader = 0;

			// Determine the next one by its name
			auto shader = line.substr(sizeof("#shader"));
			if (shader.rfind("vertex", 0) == 0)
				current_shader = GL_VERTEX_SHADER;
			else if (shader.rfind("fragment", 0) == 0)
				current_shader = GL_FRAGMENT_SHADER;
			else
				std::cerr << "Unkown shader '" << shader << "'\n";
			continue;
		}

		current_source += line + "\n";
	}

	if (current_shader != 0) 
		source_map[current_shader] = current_source;
	return source_map;
}

//Errors and other info for the shaders and the program are obtained
//with the following functions.
static std::string shader_info_log(GLuint shader)
{
	GLsizei info_log_size = 0;
	std::string info_log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
	info_log.resize(info_log_size);
	glGetShaderInfoLog(shader, info_log_size, &info_log_size, info_log.data());
	return info_log;
}

static GLuint compile(std::string source, GLenum type) 
{
	// Create shader
	GLuint shader = glCreateShader(type);

	// Load source
	const GLchar* glSource = reinterpret_cast<const GLchar*>(source.c_str());
	glShaderSource(shader, 1, &glSource, 0);

	// Compile
	GLint success = 0;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		std::cerr << std::endl << "Error compiling GLSL shader:\n";
		std::cerr << "Shader info log:" << shader_info_log(shader) << "\n";
		return 0;
	}
	return shader;
}

std::shared_ptr<Shader> Shader::construct(const std::string &file_path)
{
	auto shader = std::shared_ptr<Shader>(new Shader());
	auto source = load_source(file_path);
	if (source.find(GL_VERTEX_SHADER) == source.end() || source.find(GL_FRAGMENT_SHADER) == source.end())
	{
		std::cerr << "A shader needs both a vertex and fragment component\n";
		return nullptr;
	}
	
	shader->m_vertex_shader = compile(source[GL_VERTEX_SHADER], GL_VERTEX_SHADER);
	shader->m_fragment_shader = compile(source[GL_FRAGMENT_SHADER], GL_FRAGMENT_SHADER);
	if (!shader->m_vertex_shader || !shader->m_fragment_shader)
		return nullptr;

	if (!shader->link())
		return nullptr;
	return shader;
}

bool Shader::link()
{
	//create the program
	m_program = glCreateProgram();

	//attach the vertex and fragment shaders
	glAttachShader(m_program, m_vertex_shader);
	glAttachShader(m_program, m_fragment_shader);

	//link it all together
	glLinkProgram(m_program);

	//check for any errors with the shader program
	GLint success = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	if(!success)
	{
		std::cerr << "Error linking GLSL shaders into a shader program:\n";
		std::cerr << "Program info log:" << program_info_log() << "\n";
		return false;
	}

	return true;
}

std::string Shader::program_info_log() const
{
	GLsizei info_log_size = 0;
	std::string info_log;

	glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &info_log_size);
	info_log.resize(info_log_size);
	glGetProgramInfoLog(m_program, info_log_size, &info_log_size, info_log.data());
	return info_log;
}

void Shader::bind()
{
	glUseProgram(m_program);
}

void Shader::load_mvp(glm::mat4 mvp)
{
	if (!m_u_mvp)
		m_u_mvp = get_uniform_location("mvp");

	glUniformMatrix4fv(m_u_mvp, 1, GL_FALSE, (const GLfloat*)&mvp);
}

void Shader::load_diffuse(int slot)
{
	if (!m_u_diffuse)
		m_u_diffuse = get_uniform_location("diffuse");

	glUniform1i(m_u_diffuse, slot);
}

void Shader::load_color(vec4 color)
{
	if (!m_u_color)
		m_u_color = get_uniform_location("color");

	glUniform4fv(m_u_color, 1, (GLfloat*)&color);
}

GLuint Shader::get_uniform_location(const char* name)
{
	return glGetUniformLocation(m_program, name);
}

Shader::~Shader()
{
	glUseProgram(0);
	glDeleteShader(m_vertex_shader);
	glDeleteShader(m_fragment_shader);
	glDeleteProgram(m_program);
}
