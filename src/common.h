#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int Id;
    std::string Type;
    std::string Path;
};

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture>      Textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader &shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

class Model 
{
    public:
        explicit Model(const std::string& path, bool flipUVs = false)
        {
            loadModel(path, flipUVs);
        }
        void Draw(Shader &shader);	
    private:
        // model data
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded; 
        std::string directory;

        void loadModel(std::string path, bool flipUVs);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
};