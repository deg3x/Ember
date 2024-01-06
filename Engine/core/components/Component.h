﻿#pragma once

#include "Definitions.h"

#include <type_traits>
#include <memory>

#define MAKE_UNIQUE_COMPONENT()             \
    virtual inline bool IsUnique() override \
    {                                       \
        return true;                        \
    }                                       \

class Object;

class ENGINE_API Component
{
    friend class Object;
    
protected:
    // Handled by the AddComponent() function of Object
    // Do not modify manually
    Object* parent;

public:
    template <class Type, typename... Args>
    static std::shared_ptr<Type> CreateComponent(Args... args);

    virtual void Tick() const {}

    inline Object* GetParent() const
    {
        return parent;
    }
    
    virtual inline bool IsUnique()
    {
        return true;
    }
};

template <class Type, typename ... Args>
std::shared_ptr<Type> Component::CreateComponent(Args... args)
{
    if (!std::is_base_of<Component, Type>())
    {
        return nullptr;
    }

    return std::make_shared<Type>(args...);
}
