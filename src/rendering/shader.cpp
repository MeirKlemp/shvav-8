#include "shader.h"

#include <glad/glad.h>

namespace shvav8 {

Shader Shader::from_file(const std::string& filename) {
    std::ifstream stream(filename);
    return Shader(std::move(stream));
}

Shader::Shader(const std::string& source) : Shader(std::istringstream(source)) {}

Shader::Shader(std::basic_istream<char>&& stream) : m_prog_id(0) {
    auto [vertex_shader, fragment_shader] = parse_shader(stream);
    m_prog_id = create_shader(vertex_shader, fragment_shader);
}

Shader::Shader(Shader&& shader)
    : m_prog_id(shader.m_prog_id),
      m_uniform_location_cache(std::move(shader.m_uniform_location_cache)) {
    shader.m_prog_id = 0;
}

Shader::~Shader() {
    if (m_prog_id) {
        glDeleteProgram(m_prog_id);
    }
}

void Shader::bind() const { glUseProgram(m_prog_id); }

void Shader::unbind() const { glUseProgram(0); }

void Shader::set_uniform_1i(const std::string& name, const i32 value) {
    glUniform1i(get_uniform_location(name), value);
}

void Shader::set_uniform_1f(const std::string& name, const f32 value) {
    glUniform1f(get_uniform_location(name), value);
}

void Shader::set_uniform_2f(const std::string& name, const f32 v0, const f32 v1) {
    glUniform2f(get_uniform_location(name), v0, v1);
}

void Shader::set_uniform_4f(const std::string& name, const f32 v0, const f32 v1, const f32 v2,
                            const f32 v3) {
    glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
}

u32 Shader::get_uniform_location(const std::string& name) {
    bind();

    if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end()) {
        return m_uniform_location_cache[name];
    }

    int location = glGetUniformLocation(m_prog_id, name.c_str());

    if (location == -1) {
        std::cerr << "Warning: uniform " << name << " doesn't exist." << std::endl;
    }

    m_uniform_location_cache[name] = location;
    return location;
}

u32 Shader::compile_shader(u32 shader_type, const std::string& source) {
    u32 shader_id = glCreateShader(shader_type);
    const char* c_source = source.c_str();

    glShaderSource(shader_id, 1, &c_source, nullptr);
    glCompileShader(shader_id);

    int result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

    /* Error handling. */
    if (result == GL_FALSE) {
        std::cerr << "Failed to compile "
                  << (shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader.\n";

        int length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

        char* info = new char[length * sizeof(char)];
        glGetShaderInfoLog(shader_id, length, &length, info);
        std::cerr << info << std::endl;
        delete[] info;

        glDeleteShader(shader_id);
        return 0;
    }

    return shader_id;
}

u32 Shader::create_shader(const std::string& vertex_shader, const std::string& fragment_shader) {
    u32 program_id = glCreateProgram();
    u32 vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    u32 fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    glValidateProgram(program_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

std::pair<std::string, std::string> Shader::parse_shader(std::basic_istream<char>& stream) {
    enum class ShaderType { NONE = -1, VERTEX, FRAGMENT };

    ShaderType type = ShaderType::NONE;
    std::string line;
    std::stringstream shaders[2];
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
            continue;
        }

        if (type != ShaderType::NONE) {
            shaders[(i32)type] << line << '\n';
        }
    }

    return std::make_pair(shaders[0].str(), shaders[1].str());
}

}  // namespace shvav8
