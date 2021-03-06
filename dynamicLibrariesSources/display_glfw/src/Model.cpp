#include "Model.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include "DisplayGlfw.hpp"


Model::Model() :
		positionMax_(0.f),
		positionMin_(0.f),
		positionCenter_(0.f),
		interScaling_(1.f) {
}

Model::Model(std::string const &path) :
		positionMax_(0.f),
		positionMin_(0.f),
		positionCenter_(0.f),
		interScaling_(1.f) {
	setModel(path);
}

void	Model::setModel(std::string const &path) {
	clean_();
	path_ = path;
	loadModel_();
	setupScaling_();
}

Model::~Model() {
    clean_();
}

void    Model::clean_() {
	mesh_.clear();
}

std::deque<Mesh> const &Model::getMeshes() const {
	return (mesh_);
}


float	Model::getInterScaling() const {
	return (interScaling_);
}
glm::vec3	Model::getPositionCenterRelativeToOrigin() const {
	return (positionCenter_);
}

void	Model::render(GLenum typeOfDraw) const {
    for (const auto &i : mesh_)
		i.render(typeOfDraw);
}
void	Model::render(Shader &shader, GLenum typeOfDraw) const {
    for (const auto &i : mesh_)
		i.render(shader, typeOfDraw);
}

void 					Model::loadModel_() {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path_.generic_string(), aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_GenNormals | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
        throw(Model::ModelException(std::string("ERROR::ASSIMP::") + import.GetErrorString()));
    }
    directory_ = path_.parent_path();
    processNode_(scene->mRootNode, scene);
}

void					Model::processNode_(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++)  {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh_(mesh, scene);
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode_(node->mChildren[i], scene);
    }
}


void					Model::processMesh_(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<Texture>        textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)  {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if(mesh->mTextureCoords[0])// Each 1st column = a texture (Jusqu'a 8)
            vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.uv = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);

        if (vertex.position.x > positionMax_.x)
        	positionMax_.x = vertex.position.x;
		if (vertex.position.y > positionMax_.y)
			positionMax_.y = vertex.position.y;
		if (vertex.position.z > positionMax_.z)
			positionMax_.z = vertex.position.z;

		if (vertex.position.x < positionMin_.x)
			positionMin_.x = vertex.position.x;
		if (vertex.position.y < positionMin_.y)
			positionMin_.y = vertex.position.y;
		if (vertex.position.z < positionMin_.z)
			positionMin_.z = vertex.position.z;
    }


    for(unsigned int i = 0; i < mesh->mNumFaces; i++)  {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) //Always 3 : I make Triangular Flag Option
            indices.push_back(face.mIndices[j]);
    }

    // Traitement des matériaux
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures_(material, aiTextureType_DIFFUSE, Texture::eType::DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures_(material, aiTextureType_SPECULAR, Texture::eType::SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    mesh_.emplace_back(vertices, indices, textures);
}

void			Model::setupScaling_()  {
	glm::vec3	diff;
	float		scaling;

	diff = glm::abs(positionMax_ - positionMin_);
	if (diff.x > diff.y && diff.x > diff.z)
		scaling = 1.f / diff.x;
	else if (diff.y > diff.x && diff.y > diff.z)
		scaling = 1.f / diff.y;
	else
		scaling = 1.f / diff.z;
	diff = positionMax_ - positionMin_;
	interScaling_ = scaling;
	diff = diff * 0.5f + positionMin_;
	positionCenter_ = diff;
}

std::vector<Texture>    Model::loadMaterialTextures_(aiMaterial *mat, aiTextureType type, Texture::eType eType)
{
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)  {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.id = Texture::TextureFromFile(str.C_Str(), directory_);
        texture.type = eType;
        texture.path = std::string(str.C_Str());
        textures.push_back(texture);
    }
    return (textures);
}

unsigned int Texture::TextureFromFile(const char *path, boost::filesystem::path const &directory)
{
	boost::filesystem::path		filename(directory / path);
    unsigned char      			 *data;
    unsigned int        		textureID;
    GLenum              		format;
    int                 		width, height, nrComponents;


    glGenTextures(1, &textureID);

    data = stbi_load(filename.generic_string().c_str(), &width, &height, &nrComponents, 0);
    if (data && nrComponents == 2)
		throw(Model::ModelException(std::string("Texture cannot be GREY_ALPHA at path: ") + path));
    else if (data) {

        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = 0;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
	{
		stbi_image_free(data);
		throw(Model::ModelException(std::string("Texture failed to load at path: ") + filename.generic_string()));

	}
	stbi_image_free(data);

    return (textureID);
}

bool        Model::debug_ = true;

Model::ModelException::~ModelException() noexcept = default;

Model::ModelException::ModelException() noexcept :
		error_("Error on Glfw constructor") { }

Model::ModelException::ModelException(
		std::string s) noexcept :
		error_(s) {}

Model::ModelException::ModelException(
		Model::ModelException const &src) noexcept :
		error_(src.error_) { error_ = src.error_; }

const char *
Model::ModelException::what() const noexcept { return (error_.c_str()); }
