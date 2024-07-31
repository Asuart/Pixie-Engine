#include "pch.h"
#include "PixieEngineApp.h"

PixieEngineApp app;

void window_size_callback(GLFWwindow* window, int width, int height) {
	app.HandleResize(width, height);
}

int main() {
	glfwSetWindowSizeCallback(app.GetGLFWWindow(), window_size_callback);
	app.Start();
	return 0;
}