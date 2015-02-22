#pragma once
#include <string>
#include <fstream>
#include <sstream>

namespace util{
    inline std::string loadShader(const std::string& sourceFile){
        std::ifstream file;
        file.open(sourceFile);
        if(!file.is_open()){
            std::cerr << "Failed top open '" << sourceFile << "'" << std::endl;
            return "";
        }
        std::string slurped;
        file.seekg(0, std::ios::end);
        slurped.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&slurped[0], slurped.length());
    
        return slurped;
    }
}