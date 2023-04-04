#pragma once
#include <vector>
#include "../utils/Types.h"

class Mesh
{
protected:
	std::vector<VertexData> vertexData;
	std::vector<unsigned int> indices;

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

protected:
	void SetupMesh();

public:
	Mesh() {}
	Mesh(std::vector<VertexData> data, std::vector<unsigned int> initIndices);
	virtual ~Mesh();

	void DrawMesh() const;
};