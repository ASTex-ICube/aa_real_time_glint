// https://learnopengl.com/Model-Loading/Model

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "openglogl.h"

#include "glslprogram.h"
#include "mesh.h"
#include "texturepool.h"

class Model {
public:
	Model(const std::string& path)
	{
		texturePool = new TexturePool();
		loadModel(path);
	}
	void Draw(GLSLProgram& shader);
	void DrawMeshX(GLSLProgram& shader, int X);
	std::string getNameMeshX(int X);
private:
	// texture data
	TexturePool* texturePool;
	// model data
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<int> loadMaterialTextures(aiMaterial* mat, const aiTextureType& assimpType, const Texture::Type& type, const float& bump_factor = 1.f);
};
