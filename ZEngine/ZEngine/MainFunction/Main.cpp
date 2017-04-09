#include "../UnitTest/VectorTest.h"

#include "../ZEngine.h"
#include "../ZEGameContext.h"

#include "MainFunctions.h"

#define GLUE_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <stdio.h>

void displayMe(void) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// CLosing screen when using press Escape
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char** argv) {

	// init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create Window using GLFW
	GLFWwindow* window = glfwCreateWindow(800, 600, "ZEngine", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		ZASSERT(true, "Fail to create Window for GLFW");
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Init GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		ZASSERT(true, "Fail to init GLEW");
		return -1;
	}

	// Viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Set Callback key
	glfwSetKeyCallback(window, key_callback);

	// Init Engine
	ZE::GameContext gameContext;

	ZE::MainSetup(&gameContext);

	// Main Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		displayMe();
		glfwSwapBuffers(window);
	}

	ZE::MainClean(&gameContext);

	// Terminate and Exit
	glfwTerminate();
	return 0;

}