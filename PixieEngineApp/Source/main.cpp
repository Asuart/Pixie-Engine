#include "pch.h"
#include "PixieEngineApp.h"

PixieEngineApp* app;

void window_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	if (!app) return;
	app->HandleResize(width, height);
}

int32_t main(int32_t argc, char** argv) {
	ResourceManager::SetApplicationPath(argv[0]);
	app = new PixieEngineApp();
	glfwSetWindowSizeCallback(app->GetGLFWWindow(), window_size_callback);
	app->Start();
	delete app;
	return 0;
}