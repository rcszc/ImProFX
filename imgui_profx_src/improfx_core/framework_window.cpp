// framework_window
#include "framework_window.h"

using namespace std;
using namespace LOGCONS;

// USE NVIDIA GPU.
extern "C" { _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; }

namespace system_err {
    // glfw system callback.
    static void GLFWsys_ErrorCallback(int error, const char* description) {
        // Event Loop: print: GLFW_Error Info.
        fprintf(stderr, u8"[WindowFramework.glfw] GLFW Error %d: %s\n", error, description);
    }

    inline bool module_logerr(
        GLenum glerr,
        const char* modulename,
        const char* message
    ) {
        if (glerr != NULL) {
            // glerror => print logsystem.
            LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_window + modulename + " code: " + to_string(glerr));
            LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_window + message);
            return false;
        }
        else
            return true;
    }
}

namespace CoreModuleWindow {
#define PRESET_SCROLL_POS 25.0f
#define PRESET_SCROLL_MIN 0.0f
#define PRESET_SCROLL_MAX 50.0f

    Vector3T<float> GLFWsystemCallback::ValueMouseScroll = Vector3T<float>(PRESET_SCROLL_POS, PRESET_SCROLL_MIN, PRESET_SCROLL_MAX);
    Vector2T<float> GLFWsystemCallback::ValueMouseCursor = {};

    // 鼠标滚轮事件回调[FW]
    void GLFWsystemCallback::CallbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
        // calc scroll x:pos, y:min, z:max.
        if (ValueMouseScroll.vector_x >= ValueMouseScroll.vector_y && (ValueMouseScroll.vector_x <= ValueMouseScroll.vector_z)) {
            ValueMouseScroll.vector_x -= float(yoffset) * 2.0f;
        }
        ValueMouseScroll.vector_x = ValueMouseScroll.vector_x <= ValueMouseScroll.vector_y ? ValueMouseScroll.vector_y : ValueMouseScroll.vector_x;
        ValueMouseScroll.vector_x = ValueMouseScroll.vector_x >= ValueMouseScroll.vector_z ? ValueMouseScroll.vector_z : ValueMouseScroll.vector_x;
    }

    // 鼠标位置回调[FW]
    void GLFWsystemCallback::CallbackCursorPos(GLFWwindow* window, double xpos, double ypos) {
        ValueMouseCursor.vector_x = (float)xpos;
        ValueMouseCursor.vector_y = (float)ypos;
    }

    // 窗口关闭回调[FW]
    void GLFWsystemCallback::CallbackClose(GLFWwindow* window) {};

    bool RenderWindow::GLFWwindowCreate(WindowConfig cfgdata) {
        bool rteurn_flag = true;
        // glfw system error.
        glfwSetErrorCallback(system_err::GLFWsys_ErrorCallback);

        GLFWmonitor* CreateFlag = {};

        // create window object.
        main_window_object = glfwCreateWindow(
            cfgdata.window_width, cfgdata.window_height, cfgdata.window_name.c_str(),
            CreateFlag, nullptr
        );
        if (main_window_object == nullptr) {
            LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_window + "window create object.");
            rteurn_flag = false;
        }

        glfwMakeContextCurrent(main_window_object);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowInit(
        uint32_t version[2],
        uint32_t MSAA,
        bool profile, bool fixedw
    ) {
        bool rteurn_flag = true;
        if (glfwInit() == NULL) {
            LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_window + "window create init.");
            rteurn_flag = false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1]);

        // COMPAT PROFILE 兼容模式.
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        // glfw.version 3.2+ [profile].
        // glfw.version 3.0+
        if (version[0] > 3) {
            if (profile)
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        }
        glfwWindowHint(GLFW_RESIZABLE, !fixedw); // Fixed window size.
        glfwWindowHint(GLFW_SAMPLES, MSAA);      // Samples MSAA.

        rteurn_flag = system_err::module_logerr(
            glfwGetError(&error_message), "window_init", error_message
        );
        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowVsync(bool vswitch) {
        // 垂直同步 (vertical synchronization).
        if (vswitch)
            glfwSwapInterval(true);
        else
            glfwSwapInterval(false);
        return system_err::module_logerr(
            glfwGetError(&error_message), "window_vsync", error_message
        );
    }

    bool RenderWindow::GLFWwindowCallback() {
        bool rteurn_flag = true;

        glfwSetScrollCallback(main_window_object, CallbackScroll);
        rteurn_flag = system_err::module_logerr(
            glfwGetError(&error_message), "fw_callback(scroll)", error_message
        );

        glfwSetCursorPosCallback(main_window_object, CallbackCursorPos);
        rteurn_flag = system_err::module_logerr(
            glfwGetError(&error_message), "fw_callback(cursorpos)", error_message
        );
        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowFree() {
        // free window object.
        glfwDestroyWindow(main_window_object);
        glfwTerminate();
        LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_window + "free glfw context.");

        return system_err::module_logerr(
            glfwGetError(&error_message), "opengl_glfw_free", error_message
        );
    }

    void RenderWindow::RenderContextA() {
        glfwPollEvents();

        glfwGetFramebufferSize(main_window_object, &render_buffer.vector_x, &render_buffer.vector_y);
        glViewport(NULL, NULL, render_buffer.vector_x, render_buffer.vector_y);

        glClearColor(render_bg_color.vector_x, render_bg_color.vector_y, render_bg_color.vector_z, render_bg_color.vector_w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderWindow::RenderContextB() {
        // set render context, swap buffer.
        glfwMakeContextCurrent(main_window_object);
        glfwSwapBuffers(main_window_object);
    }
}