#pragma once

#include "Component.h"

enum class CameraProjection : uint8_t
{
	PERSPECTIVE  = 0x00,
	ORTHOGRAPHIC = 0x01
};

class ENGINE_API Camera : public Component
{
	MAKE_COMPONENT_TYPE(CAMERA)

public:
	float perspectiveFOV;
	float orthographicSize;
	float nearClipping;
	float farClipping;
	CameraProjection projectionType;
	
public:
	Camera();

	glm::mat4x4 GetViewMatrix() const;
	glm::mat4x4 GetProjectionMatrix() const;
};
