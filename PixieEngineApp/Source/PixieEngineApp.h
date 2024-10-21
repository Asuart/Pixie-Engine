#pragma once
#include "PixieEngineWindow.h"
#include "RayTracingRenderer.h"
#include "PixieEngineInterface.h"

class PixieEngineInterface;
class RayTracingRenderer;

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleResize(uint32_t width, uint32_t height);

protected:
	static const uint32_t m_maxScenePathLength = 1024;
	char m_scenePath[m_maxScenePathLength] = "../Scenes/untitled.glb";
	PixieEngineWindow m_window;
	PixieEngineInterface m_interface;
	Scene* m_scene;
	RayTracingRenderer* m_rayTracingRenderer;
	SceneRenderer* m_sceneRenderer;
	glm::ivec2 m_viewportResolution;
	FrameBuffer* m_viewportFrameBuffer;
	SceneObject* m_selectedObject = nullptr;
	bool m_rayTracingViewport = false;

	void UpdateViewportResolution(glm::ivec2 resolution);
	void ReloadScene();
	void HandleUserInput();

	friend class RayTracingRenderer;
	friend class PixieEngineInterface;
};
