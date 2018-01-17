//////////////////////////////////
//FILE FOR TESTING PURPOSES ONLY//
//////////////////////////////////

#define GLFW_INCLUDE_VULKAN
#include "GLFW\glfw3.h"
#include "Renderer.h"
#include "RenderObject.h"
#include <iostream>
#include "Addons.h"
#include "Scene.h"

int main()
{
	Renderer app(glm::vec4(0.0f, 1.0f, 0.0f, 1.0));
	app.initGLFW();
	app.splashScreen();
	app.initWindow();
	app.initVulkan();

	Cube_R redcube(glm::vec3(1.0f, 0.0f, 0.0f), &app);
	redcube.getData();
	
	Addons addons();
	
	Scene test;

	while (!glfwWindowShouldClose(app.window))
	{
		glfwPollEvents();
		test.update(&app);
	}
	return 0;
}