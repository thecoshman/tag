#pragma once

#include <string>
#include <vector>
#include <map>

#include "util/opengl.hpp"

namespace util{
    namespace gl{
        struct MappedUV {
            float fromU, fromV, toU, toV;
        };

        struct TextureAtlas{
            void load(const std::map<std::string, std::string>& textureDetails);

            void bind() const;

            MappedUV getUVCoords(const std::string& textureName) const;

            private:
            gldr::Texture2d texture;

            std::map<std::string, MappedUV> textureMapping;
        };
    }
}
