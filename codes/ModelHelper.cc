#include "ModelHelper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "third-party/stb_image.h"
struct PrivateData {
	std::unique_ptr<Assimp::Importer> importer = nullptr;
	const aiScene* scene = nullptr;
    std::string directory = std::string();
    aiMatrix4x4 CalcRealTransform(const aiNode* pNode) {
        if (pNode == nullptr) {
            return aiMatrix4x4();
        }
        auto prev = pNode->mParent;
        aiMatrix4x4 result = pNode->mTransformation;
        while (prev != nullptr) {
            result = prev->mTransformation * result;
            prev = prev->mParent;
        }
        return result;
    }
	void ProcessNode(const aiNode* pNode, std::vector<Mesh>& meshes, std::map<std::string, Texture>& textures) {
		if (pNode == nullptr || !scene) {
			return;
		}
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[pNode->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh));
            auto& curMesh = meshes.back();
            auto curTransform = CalcRealTransform(pNode);
            for (uint32_t i = 0; i < 4; i++) {
                for (uint32_t j = 0; j < 4; j++) {
                    curMesh.transform.data[i][j] = curTransform[i][j];
                }
            }
            ProcessTexture(mesh, textures);
#ifdef _DEBUG
			printf("Mesh Name : [%s]\n", mesh->mName.C_Str());
#endif
		}
		for (uint32_t i = 0; i < pNode->mNumChildren; i++) {
			ProcessNode(pNode->mChildren[i], meshes, textures);
		}
	}
	Mesh ProcessMesh(const aiMesh* pMesh) {
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
        mesh.name = pMesh->mName.C_Str();
		auto indicesNum = mesh.indicesNum;
		mesh.indices.resize(pMesh->mNumFaces * indicesNum);
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++) {
			auto& face = pMesh->mFaces[i];
			for (uint32_t j = 0; j < indicesNum; j++) {
				mesh.indices[i * indicesNum + j] = face.mIndices[j];
			}
		}
        auto& transform = mesh.transform;
        
		return mesh;
	}
    void ProcessTexture(const aiMesh* pMesh, std::map<std::string, Texture>& textures) {
        const auto material = scene->mMaterials[pMesh->mMaterialIndex];
        if (material == nullptr) {
            return;
        }
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_DIFFUSE, "diffuse");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_SPECULAR, "specular");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_AMBIENT, "ambient");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_EMISSIVE, "emissive");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_HEIGHT, "height");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_NORMALS, "normals");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_SHININESS, "shininess");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_OPACITY, "opacity");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_DISPLACEMENT, "displacement");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_LIGHTMAP, "lightmap");
        LoadMaterialTextures(textures, pMesh, material, aiTextureType_REFLECTION, "reflection");
    }

    void LoadMaterialTextures(std::map<std::string, Texture>& textures, 
        const aiMesh* pMesh, const aiMaterial* pMaterial, aiTextureType type, std::string typeName) {
        for (uint32_t i = 0; i < pMaterial->GetTextureCount(type); i++) {
            aiString str;
            pMaterial->GetTexture(type, i, &str);
            char ckey[32] = {0,};
            sprintf_s<32>(ckey, "%s_%s_%d", pMesh->mName.C_Str(), typeName.c_str(), i+1);
            std::string key(ckey);
            textures[key] = LoadTexture(str.C_Str());
#ifdef _DEBUG
            printf("Texture Name : [%s]\n", ckey);
#endif 
        }
    }
    Texture LoadTexture(const std::string& file) {
        auto newstr = file;
        for (auto& c : newstr) {
            c = (c == '\\') ? '/' : c;
        }
        newstr = newstr.substr(newstr.find_last_of('/'));
        auto fullpath = directory + "/" + newstr;
        int x, y, c;
        auto deleter = [](uint8_t* data) {
            stbi_image_free(data);
        };
        std::unique_ptr<uint8_t, void(*)(uint8_t*)> data = 
            std::unique_ptr<uint8_t, void(*)(uint8_t*)>(stbi_load(fullpath.c_str(), &x, &y, &c, 0), deleter);
        if (!data) {
            return Texture();
        }
        return Texture(data.get(), x, y, c);
    }
};

ModelHelper::ModelHelper() : data(nullptr), directory(), meshes() , filename() {}

ModelHelper::ModelHelper(const ModelHelper & old) : ModelHelper(old.filename.c_str()) {}

ModelHelper::ModelHelper(const char * file) : ModelHelper() {
	std::string path = file;
    for (auto& c : path) {
        c = (c == '\\') ? '/' : c;
    }
    filename = path;
	data = std::make_unique<PrivateData>();
	data->importer = std::make_unique<Assimp::Importer>();
	data->scene = data->importer->ReadFile(path, 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (data->scene == nullptr || data->scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || data->scene->mRootNode == nullptr) {
		printf("load model failure! file{%s}, error{%s}", path.c_str(), data->importer->GetErrorString());
		return;
	}
    directory = path.substr(0, path.find_last_of('/'));
    data->directory = directory;
	data->ProcessNode(data->scene->mRootNode, meshes, textures);
}

ModelHelper::~ModelHelper() {
	data.reset();
}
