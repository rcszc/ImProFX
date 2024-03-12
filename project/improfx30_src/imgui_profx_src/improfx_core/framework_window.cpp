// framework_window
#include "framework_window.h"

using namespace std;
using namespace LOGCONS;

// USE NVIDIA GPU.
extern "C" { _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; }

namespace system_err {
    // glfw system callback.
    static void GLFWsys_ErrorCallback(int Error, const char* description) {
        // Event Loop: print: GLFW_Error Info.
        fprintf(stderr, u8"[WindowFramework.glfw] GLFW Error %d: %s\n", Error, description);
    }

    inline bool ModuleLogger(GLenum glerr, const char* modulename, const char* message) {
        if (glerr != NULL) {
            // glerror => print logsystem.
            PushLogger(LOG_ERROR, SYSMD_LB_WINDOW, "%s code: %u", modulename, glerr);
            PushLogger(LOG_ERROR, SYSMD_LB_WINDOW, "%s", message);
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
    Vector2T<float> GLFWsystemCallback::ValueWindowSize  = Vector2T<float>(1.0f, 1.0f);

    /*
    * 获取信息:
    * "GLFWsystemCallback::CallbackScroll"     鼠标滚轮事件回调[FW]
    * "GLFWsystemCallback::CallbackCursorPos"  鼠标位置回调[FW]
    * "GLFWsystemCallback::CallbackFocus"      窗口聚焦标志回调[FW]
    * "GLFWsystemCallback::CallbackDropFiles"  向窗口内拖拽文件回调[FW]
    * "GLFWsystemCallback::CallbackWindowSize" 窗口大小回调[FW]
    */

    void GLFWsystemCallback::CallbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
        // calc scroll x:pos, y:min, z:max.
        if (ValueMouseScroll.vector_x >= ValueMouseScroll.vector_y && (ValueMouseScroll.vector_x <= ValueMouseScroll.vector_z)) {
            ValueMouseScroll.vector_x -= float(yoffset) * 2.0f;
        }
        ValueMouseScroll.vector_x = ValueMouseScroll.vector_x <= ValueMouseScroll.vector_y ? ValueMouseScroll.vector_y : ValueMouseScroll.vector_x;
        ValueMouseScroll.vector_x = ValueMouseScroll.vector_x >= ValueMouseScroll.vector_z ? ValueMouseScroll.vector_z : ValueMouseScroll.vector_x;
    }

    void GLFWsystemCallback::CallbackCursorPos(GLFWwindow* window, double xpos, double ypos) {
        ValueMouseCursor.vector_x = (float)xpos;
        ValueMouseCursor.vector_y = (float)ypos;
    }

    void GLFWsystemCallback::CallbackFocus(GLFWwindow* window, int focus) {
        ValueWindowFocus = (bool)focus;
    }

    void GLFWsystemCallback::CallbackDropFiles(GLFWwindow* window, int count, const char** paths) {
        for (int i = 0; i < count; ++i)
            FilePaths.push_back(string(paths[i]));
    }

    void GLFWsystemCallback::CallbackWindowSize(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        ValueWindowSize.vector_x = (float)width;
        ValueWindowSize.vector_y = (float)height;
    }

    // 窗口关闭回调[FW]
    void GLFWsystemCallback::CallbackClose(GLFWwindow* window) {};

    bool RenderWindow::GLFWwindowCreate(WindowConfig cfgdata) {
        bool rteurn_flag = true;
        // glfw system error.
        glfwSetErrorCallback(system_err::GLFWsys_ErrorCallback);

        GLFWmonitor* Monitor = {};

        if (cfgdata.WindowFullFlag) {
            // 创建全屏无框窗口.
            Monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* Mode = glfwGetVideoMode(Monitor);
            MainWindowObject = glfwCreateWindow(Mode->width, Mode->height, cfgdata.WindowName.c_str(), Monitor, NULL);
            ValueWindowSize.vector_x = (float)Mode->width;
            ValueWindowSize.vector_y = (float)Mode->height;
        }
        else {
            // create window object.
            MainWindowObject = glfwCreateWindow(
                cfgdata.WindowSizeWidth, cfgdata.WindowSizeHeight, cfgdata.WindowName.c_str(),
                Monitor, nullptr
            );
            ValueWindowSize.vector_x = (float)cfgdata.WindowSizeWidth;
            ValueWindowSize.vector_y = (float)cfgdata.WindowSizeHeight;
        }
        
        if (MainWindowObject == nullptr) {
            PushLogger(LOG_ERROR, SYSMD_LB_WINDOW, "window create object.");
            rteurn_flag = false;
        }

        glfwMakeContextCurrent(MainWindowObject);
        //glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowInit(
        uint32_t version[2],
        uint32_t MSAA,
        bool profile, bool fixedsize
    ) {
        bool rteurn_flag = true;
        if (glfwInit() == NULL) {
            PushLogger(LOG_ERROR, SYSMD_LB_WINDOW, "window create init.");
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
        glfwWindowHint(GLFW_RESIZABLE, !fixedsize);              // fixed window size.
        glfwWindowHint(GLFW_SAMPLES, MSAA);                      // samples MSAA.
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // 窗口透明.

        rteurn_flag = system_err::ModuleLogger(
            glfwGetError(&ErrorMessage), "window_init", ErrorMessage
        );
        return rteurn_flag;
    }

#include "stb_image.h"
    void RenderWindow::GLFWwindowSetIncon(const char* file) {
        int image_channels = NULL;

        GLFWimage iconimage;
        if (std::filesystem::exists(file)) {

            iconimage.pixels = stbi_load(file, &iconimage.width, &iconimage.height, &image_channels, NULL);
            glfwSetWindowIcon(MainWindowObject, 1, &iconimage);
            stbi_image_free(iconimage.pixels);
            PushLogger(LOG_INFO, SYSMD_LB_WINDOW, "load icon_image: %s", file);
        }
        else
            PushLogger(LOG_WARNING, SYSMD_LB_WINDOW, "failed load icon_image.");
    }

    bool RenderWindow::GLFWwindowVsync(bool vswitch) {
        // 垂直同步 (vertical synchronization).
        if (vswitch)
            glfwSwapInterval(true);
        else
            glfwSwapInterval(false);
        return system_err::ModuleLogger(
            glfwGetError(&ErrorMessage), "window_vsync", ErrorMessage
        );
    }

    bool RenderWindow::GLFWwindowCallback() {
        bool rteurn_flag = true;

        glfwSetScrollCallback(MainWindowObject, CallbackScroll);
        rteurn_flag = system_err::ModuleLogger(glfwGetError(&ErrorMessage), "fw_callback:'scroll'", ErrorMessage);

        glfwSetCursorPosCallback(MainWindowObject, CallbackCursorPos);
        rteurn_flag = system_err::ModuleLogger(glfwGetError(&ErrorMessage), "fw_callback:'cursor position'", ErrorMessage);

        glfwSetWindowFocusCallback(MainWindowObject, CallbackFocus);
        rteurn_flag = system_err::ModuleLogger(glfwGetError(&ErrorMessage), "fw_callback:'window focus'", ErrorMessage);

        glfwSetDropCallback(MainWindowObject, CallbackDropFiles);
        rteurn_flag = system_err::ModuleLogger(glfwGetError(&ErrorMessage), "fw_callback:'drop files'", ErrorMessage);

        glfwSetFramebufferSizeCallback(MainWindowObject, CallbackWindowSize);
        rteurn_flag = system_err::ModuleLogger(glfwGetError(&ErrorMessage), "fw_callback:'window resize'", ErrorMessage);

        return rteurn_flag;
    }

    bool RenderWindow::GLFWwindowFree() {
        // free window object.
        glfwDestroyWindow(MainWindowObject);
        glfwTerminate();
        PushLogger(LOG_INFO, SYSMD_LB_WINDOW, "free glfw context.");

        return system_err::ModuleLogger(
            glfwGetError(&ErrorMessage), "opengl_glfw_free", ErrorMessage
        );
    }

    void RenderWindow::RenderContextAbove() {
        glfwPollEvents();

        glfwGetFramebufferSize(MainWindowObject, &RenderBuffer.vector_x, &RenderBuffer.vector_y);
        glViewport(NULL, NULL, RenderBuffer.vector_x, RenderBuffer.vector_y);

        glClearColor(RenderBgColor.vector_x, RenderBgColor.vector_y, RenderBgColor.vector_z, RenderBgColor.vector_w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderWindow::RenderContextBelow() {
        // set render context, swap buffer.
        glfwMakeContextCurrent(MainWindowObject);
        glfwSwapBuffers(MainWindowObject);
    }
}