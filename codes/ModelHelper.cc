#include "ModelHelper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct PrivateData {
	std::unique_ptr<Assimp::Importer> importer = nullptr;
	const aiScene* scene = nullptr;
	void ProcessNode(aiNode* pNode, std::vector<Mesh>& meshes) {
		if (pNode == nullptr || !scene) {
			return;
		}
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[pNode->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh));
			printf("Mesh Name : [%s]\n", mesh->mName.C_Str());
		}
		for (uint32_t i = 0; i < pNode->mNumChildren; i++) {
			ProcessNode(pNode->mChildren[i], meshes);
		}
	}
	Mesh ProcessMesh(aiMesh* pMesh) {
		if (pMesh == nullptr) {
			return Mesh();
		}
		Mesh mesh;
		mesh.vertices.resize(pMesh->mNumVertices);
		for (uint32_t i = 0; i <pMesh->mNumVertices; i++) {
			auto& v = mesh.vertices[i];
			v.position = vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
			v.normal = vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
			if (pMesh->mTextureCoords[0]) {
				v.texCoords = vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
			}
		}
		mesh.indicesNum = pMesh->mFaces[0].mNumIndices;
		auto indicesNum = mesh.indicesNum;
		mesh.indices.resize(pMesh->mNumFaces * indicesNum);
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++) {
			auto& face = pMesh->mFaces[i];
			for (uint32_t j = 0; j < indicesNum; j++) {
				mesh.indices[i * indicesNum + j] = face.mIndices[j];
			}
		}
		return mesh;
	}
};

ModelHelper::ModelHelper() : data(nullptr), directory(), meshes() {}

ModelHelper::ModelHelper(const ModelHelper & old) : data(old.data.get()),
directory(old.directory), meshes() {}

ModelHelper::ModelHelper(const char * file) : ModelHelper() {
	data = std::make_unique<PrivateData>();
	data->importer = std::make_unique<Assimp::Importer>();
	if (!data->importer->IsExtensionSupported(".fbx")) {
		return;
	}
	data->scene = data->importer->ReadFile(file, 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (data->scene == nullptr || data->scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || data->scene->mRootNode == nullptr) {
		printf("load model failure! file{%s}, error{%s}", file, data->importer->GetErrorString());
		return;
	}
	std::string path = file;
	directory = path.substr(0, path.find_last_of('\\'));
	if (directory.size() == path.size()) {
		directory = path.substr(0, path.find_last_of('/'));
	}
	data->ProcessNode(data->scene->mRootNode, meshes);
}


ModelHelper::~ModelHelper() {
	data.reset();
}
