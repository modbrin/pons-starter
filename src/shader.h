#ifndef PONS_SHADER_H
#define PONS_SHADER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

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
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);
private:
    GLuint ID;
};


#endif //PONS_SHADER_H
