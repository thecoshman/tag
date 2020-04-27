#pragma once

#include <string>
#include <fstream>
#include <sstream>
 
namespace util{
    namespace loader{
        gldr::Texture2d loadTexture(const std::string& file){
            std::unique_ptr<glimg::ImageSet> imageSet(glimg::loaders::stb::LoadFromFile(file));
            auto image = imageSet->GetImage(0);
            auto dim = image.GetDimensions();

            gldr::Texture2d texture;
            texture.setFiltering(gldr::textureOptions::FilterDirection::Minification, gldr::textureOptions::FilterMode::Nearest);
            texture.setFiltering(gldr::textureOptions::FilterDirection::Magnification, gldr::textureOptions::FilterMode::Nearest);

            texture.imageData(dim.width, dim.height,
                gldr::textureOptions::Format::RGBA,
                gldr::textureOptions::InternalFormat::SRGB,
                gldr::textureOptions::DataType::UnsignedByte,
                image.GetImageData()
            );
            return texture;
        }
    }
}