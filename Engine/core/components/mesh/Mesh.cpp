#include "Mesh.h"

#include "glad/glad.h"
#include "../Camera.h"
#include "../Transform.h"
#include "../light/Light.h"
#include "../../Shader.h"
#include "../../Material.h"
#include "../../objects/Object.h"

#include <iostream>

Mesh::Mesh(const std::shared_ptr<Material>& initMaterial)
{
	material = initMaterial;
}

Mesh::Mesh(const std::vector<VertexData>& data, const std::vector<unsigned>& initIndices, const std::shared_ptr<Material>& initMaterial)
{
	vertexData = data;
	indices = initIndices;
	material = initMaterial;

	SetupMesh();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) *vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) *indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Mesh::Draw(const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights) const
{
	if (material == nullptr)
	{
		std::cerr << "[!] Mesh has no material!" << std::endl;
		return;
	}
	
	material->Use();

	for (const std::shared_ptr<Light>& light : lights)
	{
		light->SetShaderProperties(*material->GetShader());
	}
	
	const glm::mat4x4 model = GetParent()->transform->GetModelMatrix();
	const glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(model));
	
	material->GetShader()->SetMatrix4x4("model", model);
	material->GetShader()->SetMatrix4x4("normalMatrix", normalMatrix);
	material->GetShader()->SetVector3("cameraPosition", camera->GetParent()->transform->position);
	material->GetShader()->SetMatrix4x4("view", camera->GetViewMatrix());
	material->GetShader()->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	material->GetShader()->SetVector3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	material->GetShader()->SetVector3("material.specular", glm::vec3(0.9f, 0.8f, 0.8f));
	material->GetShader()->SetFloat("material.shininess", 64);
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
