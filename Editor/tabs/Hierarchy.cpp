﻿#include "editor_pch.h"
#include "Hierarchy.h"

#include "core/World.h"
#include "core/Object.h"
#include "input/Input.h"
#include "themes/EditorTheme.h"
#include "utils/ObjectPrimitive.h"

namespace
{
    bool anyHovered;
}

Hierarchy::Hierarchy(Editor* owner) : EditorTab(owner)
{
    title = "Hierarchy";

    SelectedObject.reset();
    hoveredObject.reset();
    clickedObject.reset();
}

void Hierarchy::Tick()
{
    EditorTab::Tick();

    std::vector<std::shared_ptr<Object>> objects;
    objects.reserve(World::worldObjects.size());
    objects.insert(objects.end(), World::worldObjects.begin(), World::worldObjects.end());
    
    if (ImGui::Begin(title.c_str(), nullptr, flags))
    {
        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 15.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {4.0f, 5.0f});
        ImGui::PushStyleColor(ImGuiCol_Header, EditorTheme::ColorGreen);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::ColorGreenHovered);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorTheme::ColorGreenActive);
        if (ImGui::TreeNodeEx("World", treeFlags))
        {
            treeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;
            
            anyHovered = false;
            
            for (const std::shared_ptr<Object>& object : objects)
            {
                if (object->GetParent() != nullptr)
                {
                    continue;
                }

                AddNode(object, treeFlags);
            }

            if (!anyHovered)
            {
                hoveredObject.reset();
            }
            
            ImGui::TreePop();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);

        DrawPopupAddObject();
        
        HandleMouseBehavior();
        
        ImGui::End();
    }
}

void Hierarchy::AddNode(const std::shared_ptr<Object>& object, ImGuiTreeNodeFlags nodeFlags)
{
    nodeFlags |= object->GetNumChildren() == 0 ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

    if (!SelectedObject.expired())
    {
        if (SelectedObject.lock() == object)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        else
        {
            nodeFlags &= ~ImGuiTreeNodeFlags_Selected;
        }
    }

    const bool isNodeOpen = ImGui::TreeNodeEx(object->name.c_str(), nodeFlags);
    
    if(ImGui::IsItemHovered())
    {
        hoveredObject = object;
        anyHovered    = true;
    }
    
    if(isNodeOpen)
    {
        for (const std::shared_ptr<Object>& child : object->GetChildren())
        {
            AddNode(child, nodeFlags);
        }
                    
        ImGui::TreePop();
    }
}

void Hierarchy::HandleMouseBehavior()
{
    if (!ImGui::IsWindowHovered())
    {
        return;
    }

    if (Input::Mouse.leftButtonPressed)
    {
        clickedObject = hoveredObject;
    }
    else if (Input::Mouse.leftButtonPressedLastFrame)
    {
        if (hoveredObject.expired())    // Clicked in empty space
        {
            SelectedObject.reset();
            clickedObject.reset();
        }
        else if (clickedObject.lock() == hoveredObject.lock())
        {
            SelectedObject = clickedObject;
        }
        else    // This is a mouse drag case. We do not handle it for now
        {
            SelectedObject.reset();
            clickedObject.reset();
        }
    }

    const bool isRightClick = Input::Mouse.rightButtonPressedLastFrame && !Input::Mouse.rightButtonPressed;
    if (isRightClick)
    {
        ImGui::OpenPopup("AddObjectPopup", ImGuiPopupFlags_None);
    }
}

void Hierarchy::DrawPopupAddObject()
{
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 1.0f);
    
    constexpr ImGuiWindowFlags popupFlags = ImGuiWindowFlags_None;
    if (ImGui::BeginPopup("AddObjectPopup", popupFlags))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 3.0f});

        ImGui::TextUnformatted("Create New Object...");

        ImGui::Separator();

        const ImVec2 entriesSize = {ImGui::GetContentRegionAvail().x, 0.0f};
        if (ImGui::Button("Cube", entriesSize))
        {
            SelectedObject = ObjectPrimitive::InstantiateCube();
        }
        if (ImGui::Button("Sphere", entriesSize))
        {
            SelectedObject = ObjectPrimitive::InstantiateSphere();
        }
        if (ImGui::Button("Plane", entriesSize))
        {
            SelectedObject = ObjectPrimitive::InstantiatePlane();
        }

        ImGui::PopStyleVar(2);
        
        ImGui::EndPopup();
    }
    
    ImGui::PopStyleVar(1);
}
