#ifndef PONS_UTILS_H
#define PONS_UTILS_H

#include <cstdio>
#include <cstring>

#include "glad/glad.h"
#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>

/// Filename can be KTX or DDS files
GLuint createTexture(char const* Filename);
GLuint textureFromFile(std::string filename, std::string directory);
glm::mat4 MyLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
glm::mat3 GetUpperLeft3x3(const glm::mat4& source);

#endif //PONS_UTILS_H
