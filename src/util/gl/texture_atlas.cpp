#include "util/gl/texture_atlas.hpp"

#include <algorithm>
#include <cmath>

#include <glimg/glimg.h>

namespace {
    // Find the smallest power of two that would hold the value
    int smallestPower2(int value, unsigned int power = 1) {
        if (static_cast<unsigned int>(value) > power) {
            return smallestPower2(value, power << 1);
        }
        return power;
    }

    int gridSizeRequired(int elements){
        auto p2 = smallestPower2(elements);
        auto root = std::sqrt(p2);
        auto intRoot = static_cast<int>(root);
        return smallestPower2(intRoot);
    }

    struct AtlasedTexture{
        std::string name;
        std::unique_ptr<glimg::ImageSet> data;

        int width() const{
            return data->GetImage(0).GetDimensions().width;
        }

        int height() const{
            return data->GetImage(0).GetDimensions().height;
        }

        char pixelChannelValue(int index) const{
            return *(((char*) data->GetImage(0).GetImageData()) + index);
        }
    };

    void blit(std::vector<char>& destination, int atlasPixelSize, std::pair<int, int> offset, const AtlasedTexture& source){
        for (int y = 0; y < source.height(); ++y){
            int desty = offset.second + y;
            for (int x = 0; x < source.width(); ++x){
                int destx = offset.first + x;

                int srcIndex = (y * source.width()) + x;
                int dstIndex = (desty * atlasPixelSize) + destx;

                // factor in we have 4 bytes per pixel
                srcIndex *= 4;
                dstIndex *= 4;
                for (int rgba = 0; rgba < 4; ++rgba){
                    destination[dstIndex + rgba] = source.pixelChannelValue(srcIndex + rgba);
                }
            }
        }
    }
}

namespace util{
    namespace gl{
        void TextureAtlas::load(const std::map<std::string, std::string>& texturesToLoad){
            std::vector<AtlasedTexture> imageData;
            int cellSize = 1;

            std::for_each(texturesToLoad.begin(), texturesToLoad.end(), [&](const std::pair<std::string, std::string>& t){
                std::cout << "Mapping '" << t.first << "' to '" << t.second << "' ";

                imageData.push_back(AtlasedTexture{
                    t.first,
                    std::unique_ptr<glimg::ImageSet>(glimg::loaders::stb::LoadFromFile(t.second))
                });

                std::cout << "size(" << imageData.back().width() << ", " << imageData.back().height() << ") ";
                auto widthP2 = smallestPower2(imageData.back().width());
                auto heightP2 = smallestPower2(imageData.back().height());
                std::cout << "p2size(" << widthP2 << ", " << heightP2 << ")\n";

                cellSize = std::max({cellSize, widthP2, heightP2});
            });

            int gridSize = gridSizeRequired(imageData.size());
            int atlasPixelSize = gridSize * cellSize;

            std::cout << "\n";
            std::cout << "Smallest grid for all textures: " << cellSize << "\n";
            std::cout << "Textures to fit into grid: " << imageData.size() << "\n";
            std::cout << "Using a " << gridSize << " by " << gridSize << " grid\n";
            std::cout << "    a " << atlasPixelSize << " by " << atlasPixelSize << " texture\n";

            std::vector<char> combinedPixels;
            int bytesPerPixel = 4; 
            combinedPixels.reserve(atlasPixelSize * atlasPixelSize * bytesPerPixel);

            for (unsigned int i = 0; i < imageData.size(); ++i){
                auto& source = imageData.at(i);
                int cellX = i % gridSize;
                int cellY = i / gridSize;
                int offsetX = cellX * cellSize;
                int offsetY = cellY * cellSize;

                std::cout << "Allocating '" << source.name << "' to grid cell (" << cellX << "," << cellY << "). ";
                std::cout << "Pixel coord (" << offsetX << "," << offsetY << ") to (" << (offsetX + source.width()) << "," << (offsetY + source.height()) << ") ";

                auto mappedUV = MappedUV{
                    static_cast<float>(offsetX)                   / atlasPixelSize,
                    static_cast<float>(offsetY)                   / atlasPixelSize,
                    static_cast<float>(offsetX + source.width())  / atlasPixelSize,
                    static_cast<float>(offsetY + source.height()) / atlasPixelSize
                };

                std::cout << "UV (" << mappedUV.fromU << "," << mappedUV.fromV << ") to (" << mappedUV.toU << "," << mappedUV.toV << ")\n";

                blit(combinedPixels, atlasPixelSize, {offsetX, offsetY}, source);
                textureMapping.set(source.name, mappedUV);
            }

            texture.setFiltering(gldr::textureOptions::FilterDirection::Minification, gldr::textureOptions::FilterMode::Nearest);
            texture.setFiltering(gldr::textureOptions::FilterDirection::Magnification, gldr::textureOptions::FilterMode::Nearest);

            texture.imageData(atlasPixelSize, atlasPixelSize,
                gldr::textureOptions::Format::RGBA,
                gldr::textureOptions::InternalFormat::SRGB,
                gldr::textureOptions::DataType::UnsignedByte,
                combinedPixels.data()
            );
        }

        void TextureAtlas::bind() const {
            texture.bind();
        }

        MappedUV TextureAtlas::getUVCoords(const std::string& textureName) const {
            auto coords = textureMapping.get(textureName);
            if (coords) {
                return coords.value();
            } else {
                std::cout << "Failed to find '" << textureName << "' in texture atlas\n";
                return {0,0,1,1};
            }
        }
    }
}