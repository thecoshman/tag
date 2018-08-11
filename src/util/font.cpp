#include "util/font.hpp"

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

std::string font::texture_file_path(){
    return font_folder + font_name + texture_extension;
}

std::string font::config_file_path(){
    return font_folder + font_name + config_extension;
}

void font::render_glyph(int glyph, glm::vec2 const& position){
    program.setUniform("position", position);
    gl::DrawArrays(gl::TRIANGLE_STRIP, glyph, 4);
}

void font::init_shaders(){
    std::string vert = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec2 in_offset;\n"
        "layout(location = 1) in vec2 in_texcoord;\n"
        "\n"
        "uniform mat4 projection;\n"
        "uniform vec2 position;\n"
        "\n"
        "out vec2 var_tc;\n"
        "\n"
        "void main() {\n"
        "    var_tc = in_texcoord;\n"
        "    vec2 pos = position + in_offset;\n"
        "    gl_Position = projection * vec4(pos, 0.0, 1.0);\n"
        "}\n";

    std::string frag = 
        "#version 330 core\n"
        "\n"
        "out vec4 out_Color;\n"
        "\n"
        "in vec2 var_tc;\n"
        "\n"
        "uniform sampler2D tex;\n"
        "\n"
        "void main () {\n"
        "    out_Color = vec4(1.0, 1.0, 1.0, 1.0) - vec4(texture(tex, var_tc).rgb, 1.0);\n"
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

void font::load(){
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
    gl::VertexAttribPointer(0, 2, gl::FLOAT, gl::FALSE, sizeof(glm::vec2) * 2, nullptr);
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, gl::FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));

    vbo.bufferData(vboData);

    init_shaders();
}

void font::pre_draw(){
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::CULL_FACE);
    gl::Enable (gl::BLEND);
    gl::BlendFunc (gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

    vao.bind();
    vbo.bind();

    program.use();
    program.setUniform("projection", glm::ortho(0.0f, 800.0f, 600.0f, 0.0f));
    program.setTexture("tex", 0);
    texture.bind(0);
}

void font::post_draw(){
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);
    gl::Disable (gl::BLEND);
}

void font::draw (std::string const& text, glm::vec2 position){
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

font::font(std::string name): font_name(name){
    load();
}