#ifndef PONS_SHADER_H
#define PONS_SHADER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "glad/glad.h"

namespace fs = std::filesystem;

class Shader {
public:
    Shader(fs::path vertexShader, fs::path fragmentShader);
    inline GLuint getID() {
        return ID;
    }
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
private:
    GLuint ID;
};


#endif //PONS_SHADER_H
