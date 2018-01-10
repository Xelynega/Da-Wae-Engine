#define GLFW_INCLUDE_VULKAN
#include "GLFW\glfw3.h"
#include "Renderer.h"
#include "RenderObject.h"
#include <iostream>
#include "Addons.h"

int main()
{
	Renderer app;
	app.initGLFW();
	app.splashScreen();
	app.initWindow();
	app.initVulkan();

	Cube_R redcube(glm::vec3(1.0f, 0.0f, 0.0f), &app);
	
	Addons addons();
	while (!glfwWindowShouldClose(app.window))
		glfwPollEvents();
	return 0;
}