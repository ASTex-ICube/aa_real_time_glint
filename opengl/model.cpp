// https://learnopengl.com/Model-Loading/Model

#include "model.h"

using std::string;
using glm::vec3;
using glm::vec2;

#include <cstdlib>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <map>

#include "stb/stb_image.h"

void Model::Draw(GLSLProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::DrawMeshX(GLSLProgram& shader, int X)
{
	meshes[X].Draw(shader);
}

std::string Model::getNameMeshX(int X)
{
	return meshes[X].name;
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/')) + '/';

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if(mesh->mTangents != NULL)
			meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	std::vector<int> diffuseTextures;
	std::vector<int> heightTextures;
	std::vector<int> slopeTextures;
	std::vector<int> secondMomentTextures;
	std::vector<int> specularTextures;
	std::vector<int> maskTextures;
	
	glm::vec2 scaleUV;
	float scaleBump;
	float microfacetRelativeArea;
	float logMicrofacetDensity;

	glm::vec3 Kd, Ks;
	float Ns;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;

		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	if (mesh->mMaterialIndex >= 0)
	{

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D buf(0.f, 0.f, 0.f);

		// Load parameters
		material->Get(AI_MATKEY_COLOR_EMISSIVE, buf);
		scaleBump = buf.r;
		scaleUV.x = buf.g;
		scaleUV.y = buf.b;

		material->Get(AI_MATKEY_COLOR_AMBIENT, buf);
		logMicrofacetDensity = buf.r;
		microfacetRelativeArea = buf.g;


		// Load Texture2D

		diffuseTextures = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, Texture::Type::Diffuse);
		
		heightTextures = loadMaterialTextures(material,
			aiTextureType_HEIGHT, Texture::Type::Height,scaleBump);

		for (auto& i : heightTextures) {
			slopeTextures.push_back(i);
			secondMomentTextures.push_back(i);
		}

		specularTextures = loadMaterialTextures(material,
			aiTextureType_SPECULAR, Texture::Type::Specular);

		maskTextures = loadMaterialTextures(material,
			aiTextureType_OPACITY, Texture::Type::Mask);


		// Load coefficient
		material->Get(AI_MATKEY_COLOR_DIFFUSE, buf);
		Kd = glm::vec3(buf.r, buf.g, buf.b);

		material->Get(AI_MATKEY_COLOR_SPECULAR, buf);
		Ks = glm::vec3(buf.r, buf.g, buf.b);

		material->Get(AI_MATKEY_SHININESS, buf.r);
		Ns = buf.r;

	}

	return Mesh(vertices, indices, texturePool,
				diffuseTextures,
				heightTextures,
				slopeTextures,
				secondMomentTextures,
				specularTextures,
				maskTextures,
				scaleUV,
				logMicrofacetDensity,
				microfacetRelativeArea,
				Kd,Ks,Ns,
				mesh->mName.C_Str());
}


std::vector<int> Model::loadMaterialTextures(aiMaterial* mat, const aiTextureType& assimpType, const Texture::Type& type, const float& bump_factor)
{
	std::vector<int> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(assimpType); i++)
	{
		aiString str;
		mat->GetTexture(assimpType, i, &str);

		textures.push_back(texturePool->Push(type, std::string(str.C_Str()), directory,bump_factor));
	}
	return textures;
}