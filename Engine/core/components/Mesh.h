#pragma once

#include <glad/glad.h>

#include "core/components/Component.h"

class Light;
class Camera;
class Material;

struct VertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec4 color;
};

enum class MeshType : uint8_t
{
	OPAQUE,
	TRANSPARENT
};

enum class PolygonMode : uint8_t
{
	FILL,
	LINE
};

enum class CullingMode : uint8_t
{
	FRONT,
	BACK,
	NONE
};

enum class DepthTestMode : uint8_t
{
	LESS,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOT_EQUAL,
	ALWAYS,
	NEVER,
	NONE
};

class ENGINE_API Mesh : public Component
{
	MAKE_COMPONENT_TYPE(MESH)
	
public:
	std::shared_ptr<Material> material;
	MeshType meshType = MeshType::OPAQUE;
	PolygonMode polygonMode = PolygonMode::FILL;
	CullingMode cullingMode = CullingMode::BACK;
	DepthTestMode depthTest = DepthTestMode::LESS;
	bool writeToDepthBuffer = true;
	
protected:
	std::vector<VertexData> vertexData;
	std::vector<unsigned int> indices;

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

public:
	Mesh() = default;
	Mesh(const std::vector<VertexData>& data, const std::vector<unsigned int>& initIndices, const std::shared_ptr<Material>& initMaterial);
	virtual ~Mesh();

	void Draw(const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights) const;
	void SetMeshData(const std::vector<VertexData>& newData, const std::vector<unsigned int>& newIndices, const std::shared_ptr<Material>& newMaterial = nullptr);

	inline std::vector<VertexData> GetVertexData() const
	{
		return vertexData;
	}

	inline std::vector<unsigned int> GetIndices() const
	{
		return indices;
	}
	
protected:
	void SetupMesh();
	void CleanupMesh();
	void SetupDepthTestMode() const;
	void SetupPolygonMode() const;
	void SetupCullingMode() const;
	void ResetRendererState() const;
};