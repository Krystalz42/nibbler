#pragma  once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <boost/filesystem.hpp>
#include <deque>
#include <bitset>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"

class Shader;

class Model {
public:

	class ModelException : public std::exception {
	public:
		ModelException() noexcept;

		explicit ModelException(std::string) noexcept;

		const char *what() const noexcept override;

		~ModelException() noexcept override;

		ModelException(ModelException const &src) noexcept;

	private:
		ModelException &
		operator=(ModelException const &rhs) noexcept;

		std::string error_;
	};
	
	Model();
	Model(std::string const &path);
	~Model();

	void	setModel(std::string const &path);
	std::deque<Mesh> const &getMeshes() const;

	float		getInterScaling() const;
	glm::vec3	getPositionCenterRelativeToOrigin() const;

	void	render(GLenum typeOfDraw = GL_TRIANGLES) const;
	void	render(Shader &shader, GLenum typeOfDraw = GL_TRIANGLES) const;

private:
	std::deque<Mesh>		mesh_;
	boost::filesystem::path	path_;
	boost::filesystem::path	directory_;

	glm::vec3				positionMax_;
	glm::vec3				positionMin_;
	glm::vec3				positionCenter_;
	float					interScaling_;

    void					clean_();
	void 					loadModel_();
	void					setupScaling_();

	void					processNode_(aiNode *node, const aiScene *scene);
	void					processMesh_(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture>	loadMaterialTextures_(aiMaterial *mat, aiTextureType type, Texture::eType eType);

    static bool				debug_;

	Model(Model const &Model) = delete;
    Model &operator=(Model const &Model) = delete;
};
