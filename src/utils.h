#ifndef PONS_UTILS_H
#define PONS_UTILS_H

#include <cstdio>
#include <cstring>

#include "glad/glad.h"
#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>

/// Filename can be KTX or DDS files
GLuint createTexture(char const* Filename);

glm::mat4 MyLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

#endif //PONS_UTILS_H
