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
            PushLogger(LOG_ERR, SYSMD_LB_WINDOW, "%s code: %u", modulename, glerr);
            PushLogger(LOG_ERR, SYSMD_LB_WINDOW, "%s", message);
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
    bool            GLFWsystemCallback::ValueWindowFocus = false;
    vector<string>  GLFWsystemCallback::FilePaths        = {};


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

    // 窗口聚焦标志回调[FW]
    void GLFWsystemCallback::CallbackFocus(GLFWwindow* window, int focus) {
        ValueWindowFocus = (bool)focus;
    }

    // 向窗口内拖拽文件回调[FW]
    void GLFWsystemCallback::CallbackDropFiles(GLFWwindow* window, int count, const char** paths) {
        for (int i = 0; i < count; ++i)
            FilePaths.push_back(string(paths[i]));
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

        //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        //const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        //main_window_object = glfwCreateWindow(mode->width, mode->height, "Fullscreen Window", monitor, NULL);

        if (main_window_object == nullptr) {
            PushLogger(LOG_ERR, SYSMD_LB_WINDOW, "window create object.");
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
            PushLogger(LOG_ERR, SYSMD_LB_WINDOW, "window create init.");
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

#include "stb_image.h"
    void RenderWindow::GLFWwindowSetIncon(const char* file) {
        int image_channels = NULL;

        GLFWimage iconimage;
        if (std::filesystem::exists(file)) {

            iconimage.pixels = stbi_load(file, &iconimage.width, &iconimage.height, &image_channels, NULL);
            glfwSetWindowIcon(main_window_object, 1, &iconimage);
            stbi_image_free(iconimage.pixels);
            PushLogger(LOG_INFO, SYSMD_LB_WINDOW, "load icon_image: %s", file);
        }
        else
            PushLogger(LOG_WARN, SYSMD_LB_WINDOW, "failed load icon_image.");
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
        rteurn_flag = system_err::module_logerr(glfwGetError(&error_message), "fw_callback(scroll)", error_message);

        glfwSetCursorPosCallback(main_window_object, CallbackCursorPos);
        rteurn_flag = system_err::module_logerr(glfwGetError(&error_message), "fw_callback(cursorpos)", error_message);

        glfwSetWindowFocusCallback(main_window_object, CallbackFocus);
        rteurn_flag = system_err::module_logerr(glfwGetError(&error_message), "fw_callback(windowfocus)", error_message);

        glfwSetDropCallback(main_window_object, CallbackDropFiles);
        rteurn_flag = system_err::module_logerr(glfwGetError(&error_message), "fw_callback(dropfiles)", error_message);

        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowFree() {
        // free window object.
        glfwDestroyWindow(main_window_object);
        glfwTerminate();
        PushLogger(LOG_INFO, SYSMD_LB_WINDOW, "free glfw context.");

        return system_err::module_logerr(
            glfwGetError(&error_message), "opengl_glfw_free", error_message
        );
    }

    void RenderWindow::RenderContextAbove() {
        glfwPollEvents();

        glfwGetFramebufferSize(main_window_object, &render_buffer.vector_x, &render_buffer.vector_y);
        glViewport(NULL, NULL, render_buffer.vector_x, render_buffer.vector_y);

        glClearColor(render_bg_color.vector_x, render_bg_color.vector_y, render_bg_color.vector_z, render_bg_color.vector_w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderWindow::RenderContextBelow() {
        // set render context, swap buffer.
        glfwMakeContextCurrent(main_window_object);
        glfwSwapBuffers(main_window_object);
    }
}