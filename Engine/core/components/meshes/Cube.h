﻿#pragma once

#include "Mesh.h"

class ENGINE_API Cube : public Mesh
{
public:
    Cube(const std::shared_ptr<Material>& initMaterial);
    virtual ~Cube() override = default;
    
    void GenerateVertexData();
    void GenerateIndices();
};