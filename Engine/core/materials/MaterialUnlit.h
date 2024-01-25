﻿#pragma once

#include "Material.h"

class TextureDiffuse;

class ENGINE_API MaterialUnlit : public Material
{
public:
    glm::vec4 diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

private:
    std::shared_ptr<TextureDiffuse> diffuseTexture;
    
public:
    MaterialUnlit();
    MaterialUnlit(const std::shared_ptr<TextureDiffuse>& texture);
    
    void Use() const override;
    void SetupShaderVariables(const Transform& objectTransform, const Camera& camera) const override;

    void SetDiffuseTexture(const std::shared_ptr<TextureDiffuse>& texture)
    {
        diffuseTexture = texture;
    }
};
