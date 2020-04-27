#pragma once

#include <string>
#include <vector>
#include <map>

#include "util/opengl.hpp"

namespace util{
    namespace gl{
        struct TextureAtlas{
            TextureAtlas(const std::map<std::string, std::string>& textureDetails);

            // for now, let it be public so that it can used as a normal cube texture
            gldr::Texture2d texture;
        };
    }
}
