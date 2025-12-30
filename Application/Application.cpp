#include <stdlib.h>

#include "VulkanApp.h"
#include <iostream>

#define APP_NAME "Vulkan Renderer"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char* argv[])
{
	sckVK::VulkanApp App(WINDOW_WIDTH, WINDOW_HEIGHT);
	App.Init(APP_NAME);
	App.Execute();

	return 0;
}