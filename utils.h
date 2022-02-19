#ifndef UTILS_H
#define UTILS_H

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include <cstdio>
#include <cstring>

#include "glad/glad.h"
#include <gli/gli.hpp>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

[[deprecated("Should be removed after proper image library selection.")]]
GLuint loadDDS(const char * imagepath){
    unsigned char header[124];

    FILE *fp;

    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
        return 0;
    }

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width	     = *(unsigned int*)&(header[12]);
    unsigned int linearSize	 = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = new unsigned char[bufsize * sizeof(unsigned char)];
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            delete[] buffer;
            return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                               0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;
    }

    delete[] buffer;

    return textureID;
}

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

#endif //UTILS_H
