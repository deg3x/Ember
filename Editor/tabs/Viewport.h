﻿#pragma once
#include "EditorTab.h"

class FrameBuffer;

class Viewport : public EditorTab
{
public:
    Viewport() = delete;
    Viewport(Editor* owner);
    virtual ~Viewport() override = default;
    
    void Tick() override;

    TabType GetType() const override
    {
        return TabType::VIEWPORT;
    }
};
