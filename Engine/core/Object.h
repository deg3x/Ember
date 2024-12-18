﻿#pragma once

#include "Definitions.h"
#include "Renderer.h"
#include "World.h"
#include "core/components/Component.h"
#include "core/components/Light.h"

class Transform;

class ENGINE_API Object
{
public:
    bool isActive;
    uint64_t uid;
    std::string name;
    std::shared_ptr<Transform> transform;

private:
    std::vector<std::shared_ptr<Component>> components;

    std::shared_ptr<Object> parent = nullptr;
    std::vector<std::shared_ptr<Object>> children;
    
    inline static uint64_t nextAvailableID = 0;
    
public:
    Object() = delete;
    Object(const char* objName);
    virtual ~Object();

    void Tick();
    void LoadModel(const char* path);
    
    static void SetParent(const std::shared_ptr<Object>& child, const std::shared_ptr<Object>& parent);

    template <class Type, typename... Args>
    std::shared_ptr<Type> CreateComponent(Args... args);

    template <class Type>
    bool HasComponent() const;

    template <class Type>
    std::shared_ptr<Type> GetComponent() const;

    template <class Type>
    std::vector<std::shared_ptr<Type>> GetComponents() const;

    template <class Type>
    void AddComponent(const std::shared_ptr<Type>& component);

    inline std::vector<std::shared_ptr<Component>> GetComponents() const
    {
        return components;
    }

    inline std::shared_ptr<Object> GetParent() const
    {
        return parent;
    }
    
    inline std::vector<std::shared_ptr<Object>> GetChildren() const
    {
        return children;
    }

    inline uint64_t GetNumChildren() const
    {
        return children.size();
    }
};

template <class Type>
void Object::AddComponent(const std::shared_ptr<Type>& component)
{
    if (!std::is_base_of<Component, Type>())
    {
        return;
    }
    
    const std::shared_ptr<Component> comp = std::dynamic_pointer_cast<Component>(GetComponent<Type>());
    if (comp != nullptr && comp->IsUnique())
    {
        return;
    }

    component->owner = this;
    components.push_back(component);

    const std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(component);
    if (light != nullptr && World::ContainsObject(uid))
    {
        Renderer::LightsAppend(light);
    }
}

template <class Type, typename... Args>
std::shared_ptr<Type> Object::CreateComponent(Args... args)
{
    const std::shared_ptr<Component> comp = std::dynamic_pointer_cast<Component>(GetComponent<Type>());
    if (comp != nullptr && comp->IsUnique())
    {
        return nullptr;
    }
    
    std::shared_ptr<Type> component = Component::CreateComponent<Type>(args...);
    if (component == nullptr)
    {
        return nullptr;
    }

    AddComponent(component);
    
    return component;
}

template <class Type>
bool Object::HasComponent() const
{
    if (!std::is_base_of<Component, Type>())
    {
        return false;
    }
    
    for (const std::shared_ptr<Component>& component : components)
    {
        if (dynamic_cast<Type*>(component.get()) == nullptr)
        {
            continue;
        }

        return true;
    }

    return false;
}

template <class Type>
std::shared_ptr<Type> Object::GetComponent() const
{
    if (!std::is_base_of<Component, Type>())
    {
        return nullptr;
    }

    for (const std::shared_ptr<Component>& component : components)
    {
        if (dynamic_cast<Type*>(component.get()) == nullptr)
        {
            continue;
        }

        return std::dynamic_pointer_cast<Type>(component);
    }

    return nullptr;
}

template <class Type>
std::vector<std::shared_ptr<Type>> Object::GetComponents() const
{
    std::vector<std::shared_ptr<Type>> comps;

    if (!std::is_base_of<Component, Type>())
    {
        return comps;
    }

    for (const std::shared_ptr<Component>& component : components)
    {
        if (dynamic_cast<Type*>(component.get()) == nullptr)
        {
            continue;
        }

        comps.push_back(std::dynamic_pointer_cast<Type>(component));
    }

    return comps;
}
