#pragma once

#include <defines.h>

#include <string>
#include <unordered_map>

namespace shvav8 {

class Shader {
   public:
    static Shader from_file(const std::string& filename);
    Shader(const std::string& source);
    Shader(Shader&& shader);
    ~Shader();

    void bind() const;
    void unbind() const;

    void set_uniform_1i(const std::string& name, int value);
    void set_uniform_1f(const std::string& name, float value);
    void set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v4);

   private:
    Shader(std::basic_istream<char>&& stream);
    u32 get_uniform_location(const std::string& name);

    static u32 compile_shader(u32 shader_type, const std::string& source);
    static u32 create_shader(const std::string& vertex_shader, const std::string& fragment_shader);
    static std::pair<std::string, std::string> parse_shader(std::basic_istream<char>& stream);

   private:
    u32 m_prog_id;
    std::unordered_map<std::string, int> m_uniform_location_cache;
};

}  // namespace shvav8
