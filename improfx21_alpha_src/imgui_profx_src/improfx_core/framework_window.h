// framework_window. [2023_10_12] RCSZ.

#ifndef _FRAMEWORK_WINDOW_H
#define _FRAMEWORK_WINDOW_H
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../improfx_log/framework_log.hpp"

#define SYSMD_LB_WINDOW "[window_glfw]: "

namespace CoreModuleWindow {

	struct WindowConfig {

		std::string WindowName;
		uint32_t WindowSizeWidth, WindowSizeHeight;
		bool WindowFullFlag;

		WindowConfig() : 
			WindowSizeWidth  (500), 
			WindowSizeHeight (300), 
			WindowName       ({}),
			WindowFullFlag   (false)
		{}
		WindowConfig(uint32_t w, uint32_t h, std::string name, bool fs) :
			WindowSizeWidth  (w),
			WindowSizeHeight (h), 
			WindowName       (name),
			WindowFullFlag   (fs)
		{}
	};

	class GLFWsystemCallback {
	protected:
		static Vector3T<float> ValueMouseScroll; // x:pos, y:min, z:max
		static Vector2T<float> ValueMouseCursor; // x:pos.x, y:pos.y
		static bool            ValueWindowFocus; // true: focused
		static Vector2T<float> ValueWindowSize;  // x:width, y:height

		static std::vector<std::string> FilePaths; // => "CallbackDropFile"

		static void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset);
		static void CallbackCursorPos(GLFWwindow* window, double xpos, double ypos);
		static void CallbackFocus(GLFWwindow* window, int focus);
		static void CallbackDropFiles(GLFWwindow* window, int count, const char** paths);
		static void CallbackWindowSize(GLFWwindow* window, int width, int height);

		static void CallbackClose(GLFWwindow* window);
	};

	class RenderWindow :public GLFWsystemCallback {
	protected:
		GLFWwindow* MainWindowObject = {};

		Vector2T<int> RenderBuffer    = Vector2T<int>(32, 32);
		Vector4T<float> RenderBgColor = Vector4T<float>();

		const char* ErrorMessage = nullptr;

		bool GLFWwindowCreate(WindowConfig cfgdata = {});

		bool GLFWwindowInit(
			uint32_t version[2],
			uint32_t MSAA,
			bool profile, bool fixedsize
		);

		void GLFWwindowSetIncon(const char* file);

		bool GLFWwindowVsync(bool vswitch);
		bool GLFWwindowCallback();

		bool GLFWwindowFree();
		
		void RenderContextAbove(); // opengl render context above.
		void RenderContextBelow(); // opengl render context below.

		// return true:close.
		bool CloseFlag() { return (bool)glfwWindowShouldClose(MainWindowObject); };
	};
}

#endif