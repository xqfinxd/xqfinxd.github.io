#pragma once
#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <vector>
#include <map>
#include <memory>
#include "Texture.h"
struct vec2 {
	vec2() : x(0), y(0) {}
	vec2(const vec2& old) : x(old.x), y(old.y) {}
	vec2(float x, float y) : x(x), y(y) {}
	float x, y;
};
struct vec3 {
	vec3() : x(0), y(0), z(0) {}
	vec3(const vec3& old) : x(old.x), y(old.y), z(old.z) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	float x, y, z;
};
struct vec4 {
	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(const vec4& old) : x(old.x), y(old.y), z(old.z), w(old.w) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	float x, y, z, w;
};
struct vertex {
	vertex() : position(), normal(), texCoords() {}
	vertex(const vertex& old) : position(old.position), normal(old.normal), texCoords(old.texCoords) {}
	vertex(const vec3& pos, const vec3& norm, const vec2& uv) : position(pos), normal(norm), texCoords(uv) {}
	vec3 position;
	vec3 normal;
	vec2 texCoords;	//maybe a vertex could storage more than one uvs.
};
struct matrix3 {
    matrix3() : data() {}
    matrix3(const matrix3& old) {
        for (uint32_t i = 0; i < 3; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                data[i][j] = old.data[i][j];
            }
        }
    }
    float data[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
};
struct matrix4 {
    matrix4() : data() {}
    matrix4(const matrix4& old) {
        for (uint32_t i = 0; i < 4; i++) {
            for (uint32_t j = 0; j < 4; j++) {
                data[i][j] = old.data[i][j];
            }
        }
    }
    float data[4][4] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    };
};
class Mesh {
public:
    Mesh() :vertices(), indices(), indicesNum(3), name() {}
    Mesh(const Mesh& old) : vertices(old.vertices), indices(old.indices), 
        indicesNum(old.indicesNum), name(old.name), transform(old.transform) {}
    ~Mesh() {
        vertices.clear();
        indices.clear();
        name.clear();
        indicesNum = 0;
    }
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
    matrix4 transform;
	uint32_t indicesNum = 3;
    std::string name;
};

struct PrivateData;

class DLL_API ModelHelper {
public:
	ModelHelper();
	ModelHelper(const ModelHelper& old);
	ModelHelper(const char* file);
	~ModelHelper();
    const std::map<std::string, Texture>& getTextures() const {
        return textures;
    }
    const std::vector<Mesh>& getMeshes() const {
        return meshes;
    }
private:
	std::unique_ptr<PrivateData> data;
    std::string filename;
	std::string directory;

	std::vector<Mesh> meshes;
    //key format:MeshName_Type_Index, for example hand_diffus_12
    std::map<std::string, Texture> textures;
};
