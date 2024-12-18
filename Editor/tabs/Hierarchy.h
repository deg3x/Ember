﻿#pragma once
#include "EditorTab.h"

class Object;

class Hierarchy : public EditorTab
{
public:
    std::weak_ptr<Object> SelectedObject;
    
private:
    std::weak_ptr<Object> hoveredObject;
    std::weak_ptr<Object> clickedObject;

public:
    Hierarchy() = delete;
    Hierarchy(Editor* owner);
    virtual ~Hierarchy() override = default;

    void Tick() override;

    TabType GetType() const override
    {
        return TabType::HIERARCHY;
    }

private:
    void AddNode(const std::shared_ptr<Object>& object, ImGuiTreeNodeFlags nodeFlags);
    void HandleMouseBehavior();
    void DrawPopupAddObject();
};
