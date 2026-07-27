#pragma once

#include <vector>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "meshObject.h"
#include "Texture.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();
	struct Material
	{
		Texture* diffuse = nullptr;
		Texture* specular = nullptr;
		Texture* normal = nullptr;
	};

	~Model();

private:

	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<Mesh*> meshList;
	std::vector<Material> materials;
	std::vector<unsigned int> meshToTex;
};

