#pragma once

#include "Component.h"
#include "glm/glm.hpp"

class ENGINE_API Camera : public Component
{
public:
	Camera() = default;
	virtual ~Camera() = default;

	glm::mat4x4 GetViewMatrix() const;
	glm::mat4x4 GetProjectionMatrix() const;
};