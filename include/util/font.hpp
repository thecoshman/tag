#pragma once

#include <map>

#include "util/opengl.hpp"

#include <glimg/glimg.h>

#include <glm/gtc/matrix_transform.hpp>

#include "pugixml.hpp"

class font{
    class font_descriptor{
        struct char_descriptor{
            int id,
                x, y,
                width, height,
                xOffset, yOffset,
                xAdvance,
                num;

            bool operator< (char_descriptor const& other){
                return id < other.id;
            }
        };

        std::map<int, char_descriptor> data_map;
        int xSize, ySize;

        int read(const pugi::xml_node& node, const std::string& attribute_name){
            return node.attribute(attribute_name.c_str()).as_int();
        }

        public:
        void load(const std::string& config_file_path) {
            pugi::xml_document doc;
            if (doc.load_file(config_file_path.c_str()).status != pugi::status_ok){
                throw std::runtime_error("File opening failed");
            }

            auto root_node = doc.first_child();

            xSize = read(root_node.child("common"), "scaleW");
            ySize = read(root_node.child("common"), "scaleH");

            int num = 0;
            for (auto char_node = root_node.child("chars").first_child(); char_node; char_node = char_node.next_sibling()){
                char_descriptor cd;

                cd.id = read(char_node, "id");

                cd.x = read(char_node, "x");
                cd.y = read(char_node, "y");

                cd.width = read(char_node, "width");
                cd.height = read(char_node, "height");

                cd.xOffset = read(char_node, "xoffset");
                cd.yOffset = read(char_node, "yoffset");

                cd.xAdvance = read(char_node, "xadvance");

                cd.num = num++;

                data_map.insert(std::make_pair(cd.id, cd));
            }
        }

        std::map<int, char_descriptor> const& data() const { return data_map; }
        int get_xsize() const { return xSize; }
        int get_ysize() const { return ySize; }
        font_descriptor(){}
    };

    gldr::dataVertexBuffer vbo;
    gldr::VertexArray vao;
    gldr::Program program;
    gldr::Texture2d texture;

    font_descriptor fd;

    std::string font_folder = "resource/fonts/";
    std::string texture_extension = ".png";
    std::string config_extension = ".xml";
    std::string font_name;

    std::string texture_file_path();

    std::string config_file_path();

    void render_glyph(int glyph, glm::vec2 const& position);

    void init_shaders();

    void load();

    void pre_draw();

    void post_draw();

    public:
    void draw (std::string const& text, glm::vec2 position);

    font(std::string name);
};