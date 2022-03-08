#include "common.h"
#include "utils.h"

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->Vertices = move(vertices);
    this->Indices = move(indices);
    this->Textures = move(textures);

    setupMesh();
}

void Mesh::Draw(Shader &shader) 
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < Textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = Textures[i].Type;
        if(name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        }
        else if(name == "texture_specular") {
            number = std::to_string(specularNr++);
        }
        else if(name == "texture_normal") {
            number = std::to_string(normalNr++);
        }
        else if(name == "texture_height") {
            number = std::to_string(heightNr++);
        }

        string texparam_name("material.");
        texparam_name.append(name);
        texparam_name.append(number);
        glUniform1i(glGetUniformLocation(shader.getID(), texparam_name.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, Textures[i].Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
} 

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), 
                 &Indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    // set default as a cleanup
    glBindVertexArray(0);
}

void Model::Draw(Shader &shader) {
    for(auto & mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::loadModel(string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate );
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vec3;
        vec3.x = mesh->mVertices[i].x;
        vec3.y = mesh->mVertices[i].y;
        vec3.z = mesh->mVertices[i].z;
        vertex.Position = vec3;

        vec3.x = mesh->mNormals[i].x;
        vec3.y = mesh->mNormals[i].y;
        vec3.z = mesh->mNormals[i].z;
        vertex.Normal = vec3;

        if (mesh->HasNormals())
        {
            vec3.x = mesh->mNormals[i].x;
            vec3.y = mesh->mNormals[i].y;
            vec3.z = mesh->mNormals[i].z;
            vertex.Normal = vec3;
        }

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec2;
            vec2.x = mesh->mTextureCoords[0][i].x; 
            vec2.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec2;
            // tangent
            if (mesh->mTangents != nullptr) {
                vec3.x = mesh->mTangents[i].x;
                vec3.y = mesh->mTangents[i].y;
                vec3.z = mesh->mTangents[i].z;
                vertex.Tangent = vec3;
            }
            // bitangent
            if (mesh->mBitangents != nullptr) {
                vec3.x = mesh->mBitangents[i].x;
                vec3.y = mesh->mBitangents[i].y;
                vec3.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vec3;
            }
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);  
        }
            
        vertices.push_back(vertex);
    }

    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material,
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        vector<Texture> normalMaps = loadMaterialTextures(material,
                                            aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        vector<Texture> heightMaps = loadMaterialTextures(material,
                                            aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    } 

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].Path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.Id = textureFromFile(str.C_Str(), directory);
            texture.Type = typeName;
            texture.Path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}  