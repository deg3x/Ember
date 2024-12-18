﻿#pragma once

#include "Definitions.h"
#include "core/Texture.h"

class Shader;
class Camera;
class Transform;

struct MaterialProperty
{
    enum class PropertyType : uint8_t
    {
        INTEGER,
        BOOLEAN,
        FLOAT,
        VECTOR3,
        VECTOR4,
        MATRIX4X4
    };

    union PropertyValue
    {
        int intVal;
        bool boolVal;
        float floatVal;
        Vector3 vector3Val;
        Vector4 vector4Val;
        Matrix4x4 matrixVal;
    };

    std::string name;
    PropertyValue value;
    PropertyType type;

    MaterialProperty() : name(""), value({.intVal = 0}), type(PropertyType::INTEGER) {}
    MaterialProperty(const std::string& newName, int newValue) : name(newName), value({.intVal = newValue}), type(PropertyType::INTEGER) {}
    MaterialProperty(const std::string& newName, bool newValue) : name(newName), value({.boolVal = newValue}), type(PropertyType::BOOLEAN) {}
    MaterialProperty(const std::string& newName, float newValue) : name(newName), value({.floatVal = newValue}), type(PropertyType::FLOAT) {}
    MaterialProperty(const std::string& newName, const Vector3& newValue) : name(newName), value({.vector3Val = newValue}), type(PropertyType::VECTOR3) {}
    MaterialProperty(const std::string& newName, const Vector4& newValue) : name(newName), value({.vector4Val = newValue}), type(PropertyType::VECTOR4) {}
    MaterialProperty(const std::string& newName, const Matrix4x4& newValue) : name(newName), value({.matrixVal = newValue}), type(PropertyType::MATRIX4X4) {}

    ////// Unordered set required functionality
    bool operator==(const MaterialProperty& other) const
    {
        return name == other.name;
    }
    
    size_t operator()(const MaterialProperty& materialProperty) const
    {
        return std::hash<std::string>()(materialProperty.name);
    }
    //////
};

struct MaterialTexture
{
    std::string name;
    std::shared_ptr<Texture> texture;

    MaterialTexture() = default;
    MaterialTexture(const std::string& newName, const std::shared_ptr<Texture>& newTexture) : name(newName), texture(newTexture) {}

    ////// Unordered set required functionality
    bool operator==(const MaterialTexture& other) const
    {
        return name == other.name;
    }
    
    size_t operator()(const MaterialTexture& materialTexture) const
    {
        return std::hash<std::string>()(materialTexture.name);
    }
    //////
};

class ENGINE_API Material
{
protected:
    std::unordered_set<MaterialProperty, MaterialProperty> properties;
    std::unordered_set<MaterialTexture, MaterialTexture> textures;
    std::shared_ptr<Shader> shader;

public:
    Material() = delete;
    Material(const std::shared_ptr<Shader>& initShader);

    virtual void Use() const;
    virtual void SetupShaderVariables(const Transform& objectTransform, const Camera& camera) const;

    template <class Type>
    void SetProperty(const std::string& name, const Type& value);
    void SetTexture(const std::string& name, const std::string& path, TextureType type);
    void SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture);

    // Wrappers for the respective shader functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVector3(const std::string& name, const Vector3& vector) const;
    void SetVector4(const std::string& name, const Vector4& vector) const;
    void SetMatrix4x4(const std::string& name, const Matrix4x4& matrix) const;

    inline void SetShader(const std::shared_ptr<Shader>& newShader)
    {
        shader = newShader;
    }

    inline void ClearProperties()
    {
        properties.clear();
    }
    
    inline void ClearTextures()
    {
        textures.clear();
    } 
    
    inline std::shared_ptr<Shader> GetShader() const
    {
        return shader;
    }

    inline std::unordered_set<MaterialProperty, MaterialProperty> GetProperties() const
    {
        return properties;
    }

    inline std::unordered_set<MaterialTexture, MaterialTexture> GetTextures() const
    {
        return textures;
    }
};

template <class Type>
void Material::SetProperty(const std::string& name, const Type& value)
{
    const MaterialProperty property = {name, value};
    if (properties.contains(property))
    {
        properties.extract(property);
    }
    
    properties.insert(property);
}
