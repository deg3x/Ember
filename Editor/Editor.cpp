﻿#include "Editor.h"

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "core/Scene.h"
#include "core/Window.h"
#include "core/Renderer.h"
#include "core/objects/Object.h"
#include "core/components/Camera.h"
#include "core/components/Transform.h"

#include "tabs/EditorTab.h"
#include "tabs/Viewport.h"
#include "tabs/MainMenuBar.h"

#include <memory>

#include "imgui/imgui_internal.h"
#include "tabs/Console.h"
#include "tabs/Hierarchy.h"
#include "tabs/Inspector.h"

namespace
{
	void DrawExamples(bool draw, const ImGuiIO& io)
	{
		if(!draw)
		{
			return;
		}

		bool show_another_window = true;
		bool showDemo = true;
		ImGui::ShowDemoWindow(&showDemo);
		float color[3];
		
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");								// Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");					// Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemo);					// Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);     // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", color);	// Edit 3 floats representing a color

			if (ImGui::Button("Button"))								// Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}
}

Editor::Editor()
{
	// Window needs to be initialized before the renderer
	Window::Initialize();
	Renderer::Initialize();
	
	tabs.emplace_back(std::make_shared<MainMenuBar>(this));
	tabs.emplace_back(std::make_shared<Viewport>(this));
	tabs.emplace_back(std::make_shared<Inspector>(this));
	tabs.emplace_back(std::make_shared<Console>(this));
	tabs.emplace_back(std::make_shared<Hierarchy>(this));

	IMGUI_CHECKVERSION();
	
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigWindowsResizeFromEdges  = true;
	io.ConfigViewportsNoTaskBarIcon  = true;
	io.ConfigViewportsNoDecoration   = true;
	io.IniFilename  = "editor.ini";

	windowFlags  = 0x0;
	windowFlags |= ImGuiWindowFlags_NoDocking;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlags |= ImGuiWindowFlags_NoNavFocus;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		style.WindowPadding = ImVec2(0.0f, 0.0f);
	}

	ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

Editor::~Editor()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}
}

void Editor::Tick()
{
    Scene scene;
	scene.Use();

	float theta  = -glm::quarter_pi<float>();
	float phi    = -glm::half_pi<float>();
	float radius = (float)scene.GetCamera()->GetParent()->transform->position.length();
	
	ImVec4 clear_color = ImVec4(0.16f, 0.15f, 0.18f, 1.00f);

	Renderer::SetDepthTestEnabled(true);
	Renderer::SetDepthTestFunc(GL_LESS);
	Renderer::SetBlendingEnabled(true);
	Renderer::SetBlendingFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	Renderer::SetBlendingOp(GL_FUNC_ADD);
	
	Renderer::SetClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	
	while (!Window::ShouldClose())
	{
		Window::ProcessUserInput();

		EditorRenderBegin();

		const MouseData mouse = Window::GetMouseData();

		theta  += (float)mouse.leftMouseXOffset * mouse.sensitivity;
		phi	   += (float)mouse.leftMouseYOffset * mouse.sensitivity;
		radius -= (float)mouse.rightMouseYOffset * mouse.sensitivity;

		scene.GetCamera()->GetParent()->transform->position.x = radius * glm::cos(theta) * glm::sin(phi);
		scene.GetCamera()->GetParent()->transform->position.z = radius * glm::sin(theta) * glm::sin(phi);
		scene.GetCamera()->GetParent()->transform->position.y = radius * glm::cos(phi);

		Window::ResetMouseOffsetData();

		Renderer::Clear();
		
		scene.Tick();
		
		EditorRenderEnd();
		
		Window::SwapBuffers();
	}
}

void Editor::EditorRenderBegin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	// begin window
	std::string name = "Editor";
	bool open = true;
	    
	ImGui::Begin(name.c_str(), &open, windowFlags);
	ImGui::PopStyleVar(2);

	// Create dock space parent window
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		const auto window_id = ImGui::GetID(name.c_str());
		ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	}

	for (const std::shared_ptr<EditorTab>& tab : tabs)
	{
		tab->Tick();
	}

	ImGui::End();
	
	DrawExamples(false, ImGui::GetIO());

	ImGui::Render();
}

void Editor::EditorRenderEnd()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	const ImGuiIO& io = ImGui::GetIO();
	
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
