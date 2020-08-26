#pragma once

#include <vector>
#include <memory>
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
class Mesh {
public:
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t indicesNum = 3;
};

struct PrivateData;

class __declspec(dllexport) ModelHelper {
public:
	ModelHelper();
	ModelHelper(const ModelHelper& old);
	ModelHelper(const char* file);
	~ModelHelper();
private:
	std::unique_ptr<PrivateData> data;
	std::string directory;
	std::vector<Mesh> meshes;
};
