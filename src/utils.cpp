#include "utils.h"
#include <iostream>
#include <map>
#include <optional>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "common.h"

/// Filename can be KTX or DDS files
GLuint createTexture(char const* Filename)
{
    gli::texture Texture = gli::load(Filename);
    if(Texture.empty())
        return 0;

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());

    GLuint TextureID = 0;
    glGenTextures(1, &TextureID);
    glBindTexture(Target, TextureID);
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

    glm::tvec3<GLsizei> const Extent(Texture.extent());
    GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

    switch(Texture.target())
    {
        case gli::TARGET_1D:
            glTexStorage1D(
                    Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
            break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            glTexStorage2D(
                    Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                    Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            glTexStorage3D(
                    Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                    Extent.x, Extent.y,
                    Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
            break;
        default:
            assert(0);
            break;
    }

    for(std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
        for(std::size_t Face = 0; Face < Texture.faces(); ++Face)
            for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
            {
                GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                glm::tvec3<GLsizei> Extent(Texture.extent(Level));
                Target = gli::is_target_cube(Texture.target())
                         ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                         : Target;

                switch(Texture.target())
                {
                    case gli::TARGET_1D:
                        if(gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage1D(
                                    Target, static_cast<GLint>(Level), 0, Extent.x,
                                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                    Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage1D(
                                    Target, static_cast<GLint>(Level), 0, Extent.x,
                                    Format.External, Format.Type,
                                    Texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_1D_ARRAY:
                    case gli::TARGET_2D:
                    case gli::TARGET_CUBE:
                        if(gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage2D(
                                    Target, static_cast<GLint>(Level),
                                    0, 0,
                                    Extent.x,
                                    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                    Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage2D(
                                    Target, static_cast<GLint>(Level),
                                    0, 0,
                                    Extent.x,
                                    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                    Format.External, Format.Type,
                                    Texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_2D_ARRAY:
                    case gli::TARGET_3D:
                    case gli::TARGET_CUBE_ARRAY:
                        if(gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage3D(
                                    Target, static_cast<GLint>(Level),
                                    0, 0, 0,
                                    Extent.x, Extent.y,
                                    Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                    Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage3D(
                                    Target, static_cast<GLint>(Level),
                                    0, 0, 0,
                                    Extent.x, Extent.y,
                                    Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                    Format.External, Format.Type,
                                    Texture.data(Layer, Face, Level));
                        break;
                    default: assert(0); break;
                }
            }
    return TextureID;
}

GLuint textureFromFile(std::string filename, std::string directory)
{
    std::string filepath = directory + "/" + filename;
    return createTexture(filepath.c_str());
}

glm::mat4 MyLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    glm::vec3 cameraDirection = glm::normalize(eye - center);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
    float rotationMatSrc[] = {
            cameraRight.x, cameraUp.x, cameraDirection.x, 0.0f,
            cameraRight.y, cameraUp.y, cameraDirection.y, 0.0f,
            cameraRight.z, cameraUp.z, cameraDirection.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
    };
    float translationMatSrc[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -eye.x, -eye.y, -eye.z, 1.0f,
    };
    glm::mat4 rotationMat = glm::make_mat4(rotationMatSrc);
    glm::mat4 translationMat = glm::make_mat4(translationMatSrc);
    return rotationMat * translationMat;
}

glm::mat3 GetUpperLeft3x3(const glm::mat4& source)
{
    auto *pSource = (const float*)glm::value_ptr(source);
    float values[9];
    for(size_t i = 0; i < 3; ++i) {
        for(size_t j = 0; j < 3; ++j) {
            values[i * 3 + j] = pSource[i * 4 + j];
        }
    }
    return glm::make_mat3(values);
}

std::optional<std::map<char, Character>> loadFonts(const std::string& path, unsigned int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return {};
    }

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return {};
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    std::map<char, Character> Characters;

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return {Characters};
}
