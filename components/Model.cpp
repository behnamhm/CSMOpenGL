#include "Model.h"

Model::Model()
{
}

void Model::RenderModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];
		Material& mat = materials[materialIndex];

		if (mat.diffuse)
			mat.diffuse->UseTexture(GL_TEXTURE8);

		if (mat.specular)
			mat.specular->UseTexture(GL_TEXTURE9);

		meshList[i]->Render();
	}
}

void Model::LoadModel(const std::string& fileName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		fileName,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_PreTransformVertices);
	if (!scene)
	{
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
		return;
	}

	LoadNode(scene->mRootNode, scene);

	LoadMaterials(scene);
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;


	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		// Position
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);



		// Normal
		if (mesh->HasNormals())
		{
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(1.0f);
			vertices.push_back(0.0f);
		}

		// UV coordinates
		if (mesh->mTextureCoords[0])
		{
			vertices.push_back(mesh->mTextureCoords[0][i].x);
			vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}
	}
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh();
	newMesh->Create(vertices, indices);
	meshList.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	materials.resize(scene->mNumMaterials);


	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		Material& mat = materials[i];

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("assets/textures/") + filename;

				mat.diffuse = new Texture(texPath.c_str());

				if (!mat.diffuse->LoadTexture())
				{
					delete mat.diffuse;
					mat.diffuse = nullptr;
				}


			}
			
		}
		if (material->GetTextureCount(aiTextureType_SPECULAR))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("assets/textures/") + filename;

				mat.specular = new Texture(texPath.c_str());

				if (!mat.specular->LoadTexture())
				{
					delete mat.specular;
					mat.specular = nullptr;
				}
			}
		}
		
	}
}

void Model::ClearModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

}

Model::~Model()
{
}
