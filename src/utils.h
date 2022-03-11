#ifndef PONS_UTILS_H
#define PONS_UTILS_H

#include <cstdio>
#include <cstring>
#include <optional>
#include <map>

#include "glad/glad.h"
#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.h"

/// Filename can be KTX or DDS files
GLuint createTexture(char const* Filename);
GLuint textureFromFile(std::string filename, std::string directory);
glm::mat4 MyLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
glm::mat3 GetUpperLeft3x3(const glm::mat4& source);
std::optional<std::map<char, Character>> loadFonts(const std::string& path, unsigned int fontSize = 48);

#endif //PONS_UTILS_H
