// framework_window. [2023_10_12] RCSZ.

#ifndef _FRAMEWORK_WINDOW_H
#define _FRAMEWORK_WINDOW_H
#include <GLFW/glfw3.h>

#include "../improfx_log/framework_log.hpp"

#define SYSTEM_MODULE_LABLE_window std::string("[glfw_window]: ")

namespace CoreModuleWindow {

	struct WindowConfig {

		std::string window_name;
		uint32_t window_width, window_height;

		WindowConfig() : 
			window_width  (500), 
			window_height (300), 
			window_name   ({})
		{}
		WindowConfig(uint32_t w, uint32_t h, std::string name) :
			window_width  (w),
			window_height (h), 
			window_name   (name)
		{}
	};

	class GLFWsystemCallback {
	protected:
		static Vector3T<float> ValueMouseScroll; // x:pos, y:min, z:max
		static Vector2T<float> ValueMouseCursor; // x:pos.x, y:pos.y

		static void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset);
		static void CallbackCursorPos(GLFWwindow* window, double xpos, double ypos);
		static void CallbackClose(GLFWwindow* window);
	};

	class RenderWindow :public GLFWsystemCallback {
	protected:
		GLFWwindow* main_window_object = {};

		Vector2T<int> render_buffer     = Vector2T<int>(32, 32);
		Vector4T<float> render_bg_color = Vector4T<float>();

		const char* error_message = nullptr;

		bool GLFWwindowCreate(WindowConfig cfgdata = {});

		bool GLFWwindowInit(
			uint32_t version[2],
			uint32_t MSAA,
			bool profile, bool fixedw
		);

		bool GLFWwindowVsync(bool vswitch);
		bool GLFWwindowCallback();

		bool GLFWwindowFree();
		
		void RenderContextA(); // opengl render context above
		void RenderContextB(); // opengl render context below

		// True: close.
		bool CloseFlag() { return (bool)glfwWindowShouldClose(main_window_object); };
	};
}

#endif