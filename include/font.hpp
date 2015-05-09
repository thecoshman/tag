#pragma once

#include <glimg/glimg.h>
#include "pugixml.hpp"
#include "program.hpp"
#include "vertexArray.hpp"
#include "vertexBuffer.hpp"
#include "texture.hpp"

namespace {
    gldr::Texture2d load_texture(const std::string& file){
        std::unique_ptr<glimg::ImageSet> imageSet(glimg::loaders::stb::LoadFromFile(file));
        auto image = imageSet->GetImage(0);
        auto dim = image.GetDimensions();

        gldr::Texture2d texture;
        texture.setFiltering(gldr::textureOptions::FilterDirection::Minification, gldr::textureOptions::FilterMode::Linear);
        texture.setFiltering(gldr::textureOptions::FilterDirection::Magnification, gldr::textureOptions::FilterMode::Linear);

        texture.imageData(dim.width, dim.height,
            gldr::textureOptions::Format::GreyScale,
            gldr::textureOptions::InternalFormat::GreyScale,
            gldr::textureOptions::DataType::UnsignedByte,
            image.GetImageData()
        );
        return texture;
    }
}

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

    std::string texture_file_path(){ return font_folder + font_name + texture_extension; }

    std::string config_file_path(){ return font_folder + font_name + config_extension; }

    void render_glyph(int glyph, glm::vec2 const& position){
        program.setUniform("Position", position);
        gl::DrawArrays(gl::TRIANGLE_STRIP, glyph, 4);
    }

    void init_shaders(){
        std::string vert = 
            "#version 330 core\n"
            "\n"
            "layout(location = 0) in vec2 in_offset;\n"
            "layout(location = 1) in vec2 in_texcoord;\n"
            "\n"
            "uniform mat4 Projection;\n"
            "uniform vec2 Position;\n"
            "\n"
            "out vec2 var_tc;\n"
            // "out vec2 var_accumTc;\n"
            "\n"
            "void main() {\n"
            "    var_tc = in_texcoord;\n"
            // "    var_accumTc = vec2((Position.x + in_offset.x)/800.0, (1-(Position.y + in_offset.y))/600.0);\n"
            "    vec2 pos = Position + in_offset;\n"
            "    gl_Position = Projection * vec4(pos, 0.0, 1.0);\n"

            // "    vec2 pos = Position + in_offset - vec2(400, 300);\n"
            // "    vec2 pos = Position + in_offset - vec2(800, 600);\n"
            // "    pos /= vec2(400, 300);\n"n"
            // "    pos /= vec2(800, 600);\n"
            // "    gl_Position = vec4(pos, 0, 1);\n"
            "}\n";

        std::string frag = 
            "#version 330 core\n"
            "\n"
            "out vec4 out_Color;\n"
            "\n"
            "in vec2 var_tc;\n"
            // "in vec2 var_accumTc;\n"
            "\n"
            "uniform sampler2D tex;\n"
            // "uniform sampler2D accum;\n"
            "\n"
            "void main () {\n"
            // "    out_Color = (1 - vec4(texture(tex, var_tc).rgb, 1.0)) * vec4(texture(accum, var_accumTc).rgb, 1.0);\n"
            // "    out_Color = vec4(1.0, 1.0, 1.0, 1.0) * (1 - vec4(texture(tex, var_tc).rgb, 1.0));\n"
            "    out_Color = vec4(1.0, 1.0, 1.0, 1.0) - vec4(texture(tex, var_tc).rgb, 1.0);\n"
            // "    out_Color = vec4(texture(tex, var_tc).rgb, 1.0);\n"
            // "    out_Color = texture(tex, var_tc);\n"
            // "    out_Color = vec4(0.5, 1.0, 0.5, 1.0);\n"
            // "    out_Color = vec4(var_tc, 0.0, 1.0);\n"
            //Get red texture color
            "    vec4 red = texture(tex, var_tc);\n"
            //Set alpha fragment
            "    out_Color = vec4( 1.0, 1.0, 1.0, red.r) * vec4(0.0, 0.0, 0.0, 1.0);\n"
            "}\n";

        gldr::VertexShader vertexShader(vert);
        if(!vertexShader.didCompile()){
            std::cout << "font vertex shader failed to compile\n";
        }
        gldr::FragmentShader fragmentShader(frag);
        if(!fragmentShader.didCompile()){
            std::cout << "font fragment shader failed to compile\n";
        }        
        program.attach(vertexShader, fragmentShader);
        program.link();
        
        if(!program.didLink()){
            std::cout << "font program failed to link\n    >" << program.getLog() << "\n";
        }
        program.use();
        program.setTexture("tex", 0);
    }

    void load(){
        texture = load_texture(texture_file_path());
        fd.load(config_file_path());

        std::vector<glm::vec2> vboData;

        // vbo "position" is character offset [+ character size]
        // texture coords are normalized texture coordinates

        // Texture size from descriptor
        glm::vec2 ts ((float)fd.get_xsize(), (float)fd.get_ysize());
        for (auto p : fd.data()) {
            vboData.push_back(glm::vec2(p.second.xOffset, p.second.yOffset));
            vboData.push_back(glm::vec2(p.second.x, p.second.y) / ts);

            vboData.push_back(glm::vec2(p.second.xOffset + p.second.width, p.second.yOffset));
            vboData.push_back(glm::vec2(p.second.x + p.second.width, p.second.y) / ts);

            vboData.push_back(glm::vec2(p.second.xOffset, p.second.yOffset + p.second.height));
            vboData.push_back(glm::vec2(p.second.x, p.second.y + p.second.height) / ts);

            vboData.push_back(glm::vec2(p.second.xOffset + p.second.width, p.second.yOffset + p.second.height));
            vboData.push_back(glm::vec2(p.second.x + p.second.width, p.second.y + p.second.height) / ts);
        }

        vao.bind();
        vbo.bind();

        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, nullptr);
        gl::EnableVertexAttribArray(1);
        gl::VertexAttribPointer(1, 2, gl::FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));

        vbo.bufferData(vboData);

        init_shaders();
    }

    void pre_draw(){
        gl::Disable(gl::DEPTH_TEST);
        gl::Disable(gl::CULL_FACE);
        gl::Enable (gl::BLEND);
        gl::BlendFunc (gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

        vao.bind();
        vbo.bind();

        program.use();
        program.setUniform("Projection", glm::ortho(0.0f, 800.0f, 600.0f, 0.0f));
        program.setTexture("tex", 0);
        texture.bind(0);
    }

    void post_draw(){
        gl::Enable(gl::DEPTH_TEST);
        gl::Enable(gl::CULL_FACE);
        gl::Disable (gl::BLEND);
    }

    public:
    void draw (std::string const& text, glm::vec2 position){
        pre_draw();

        for (auto c : text){
            auto it = fd.data().find(c);
            if (it != fd.data().end()){
                auto const& glyph = it->second;

                render_glyph(glyph.num * 4, position);
                position.x += glyph.xAdvance;
            }
        }

        post_draw();
    }

    font(std::string name): font_name(name){ load(); }
};