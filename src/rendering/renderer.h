#pragma once

#include <glad/glad.h>
#include <rendering/window.h>

namespace shvav8 {

/**
 * OpenGL renderer using glad.
 * This class is a singleton.
 * Should be created before usage with create method.
 */
class Renderer {
   public:
    static Renderer& create();
    static Renderer& get();

    void clear_screen(f32 r, f32 g, f32 b, f32 a = 1.0f);

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

   private:
    Renderer() = default;
    static Renderer& create_impl(bool create = true);
};

}  // namespace shvav8
