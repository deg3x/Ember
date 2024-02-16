﻿#include "engine_pch.h"
#include "World.h"

#include "glad/glad.h"

#include "components/Mesh.h"
#include "core/Object.h"
#include "core/Renderer.h"
#include "core/components/Camera.h"
#include "core/components/Light.h"
#include "core/components/Transform.h"
#include "utils/Types.h"
#include "utils/ObjectPrimitive.h"

std::unordered_set<std::shared_ptr<Object>> World::worldObjects;

void World::Initialize()
{
    const std::shared_ptr<Object> dirLightObject = std::make_shared<Object>("Directional Light");
    dirLightObject->CreateComponent<Light>();
    dirLightObject->transform->rotation.x = 30.0f;
    dirLightObject->transform->rotation.y = -30.0f;

    AddObject(dirLightObject);

    ObjectPrimitive::InstantiateSkybox();
    ObjectPrimitive::InstantiateSphere();
    ObjectPrimitive::InstantiateEditorGrid();

    // Startup renderer state
    constexpr Color clear_color = {0.16f, 0.15f, 0.18f, 1.00f};
    Renderer::SetDepthTestEnabled(true);
    Renderer::SetDepthTestFunc(GL_LESS);
    Renderer::SetBlendingEnabled(true);
    Renderer::SetBlendingFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    Renderer::SetBlendingOp(GL_FUNC_ADD);
    Renderer::SetClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
}

void World::Tick()
{
    
}

void World::AddObject(const std::shared_ptr<Object>& object)
{
    if (worldObjects.contains(object))
    {
        return;
    }

    worldObjects.insert(object);

    const std::shared_ptr<Mesh> meshComponent = object->GetComponent<Mesh>();
    if (meshComponent != nullptr)
    {
        if (meshComponent->meshType == MeshType::TRANSPARENT)
        {
            Renderer::RenderQueueAppend(meshComponent);
        }
        else
        {
            Renderer::RenderQueuePrepend(meshComponent);
        }
    }
    
    const std::shared_ptr<Camera> cameraComponent = object->GetComponent<Camera>();
    if (cameraComponent != nullptr)
    {
        // Special handling of camera
    }

    const std::shared_ptr<Light> lightComponent = object->GetComponent<Light>();
    if (lightComponent != nullptr)
    {
        Renderer::LightsAppend(lightComponent);
    }
}

void World::RemoveObject(const std::shared_ptr<Object>& object)
{
    if (worldObjects.contains(object))
    {
        const std::shared_ptr<Light> lightComponent = object->GetComponent<Light>();
        const std::shared_ptr<Mesh> meshComponent   = object->GetComponent<Mesh>();

        if (meshComponent != nullptr)
        {
            Renderer::RenderQueueRemove(meshComponent);
        }
        if (lightComponent != nullptr)
        {
            Renderer::LightsRemove(lightComponent);
        }
        
        worldObjects.erase(object);
    }
}
