#pragma once

#include <glload/gl_3_3.hpp>
#include <glload/gl_load.hpp>

namespace gl {
    // These appear to have been missed by glload
    [[maybe_unused]] const GLint TRUE = 1;
    [[maybe_unused]] const GLint FALSE = 0;
}

#include "program.hpp"
#include "texture.hpp"
#include "textureCube.hpp"
#include "vertexArray.hpp"
#include "vertexBuffer.hpp"