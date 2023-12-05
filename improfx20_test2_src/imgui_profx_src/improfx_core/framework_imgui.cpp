// framework_imgui.
#include "framework_imgui.h"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleImGui {

    void RenderImGui::ImGuiInit(GLFWwindow* window_object, GuiConfig cfgdata) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& GUIIO = ImGui::GetIO(); (void)GUIIO;
        GUIIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // enable keyboard controls.
        GUIIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // enable gamepad controls.

        if (!std::filesystem::exists(cfgdata.fonts_filepath))
            PushLogger(LOG_WARN, SYSMD_LB_IMGUI, "failed read fonts file.");
        else
            PushLogger(LOG_INFO, SYSMD_LB_IMGUI, "read fonts file: ", cfgdata.fonts_filepath);

        // setup imgui style.
        ImGui::StyleColorsDark();
        {
            ImGuiStyle* config_style = &ImGui::GetStyle();
            ImVec4* config_colors = config_style->Colors;

            config_colors[ImGuiCol_Text] = ImVec4(
                cfgdata.fonts_color.vector_x,
                cfgdata.fonts_color.vector_y,
                cfgdata.fonts_color.vector_z,
                cfgdata.fonts_color.vector_w
            );
        }

        // init set font.
        auto set_fonts = ImGui::GetIO().Fonts;
        set_fonts->AddFontFromFileTTF(
            cfgdata.fonts_filepath.c_str(),
            cfgdata.fonts_size,
            NULL,
            set_fonts->GetGlyphRangesChineseFull()
        );
        ImGui::GetIO().FontGlobalScale = 0.55f;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, cfgdata.window_rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfgdata.frame_rounding);

        // setup platform / renderer backends.
        ImGui_ImplGlfw_InitForOpenGL(window_object, true);
        ImGui_ImplOpenGL3_Init(cfgdata.shader_version.c_str());
    }

    void RenderImGui::ImGuiFree() {
        // end close imgui.
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        PushLogger(LOG_INFO, SYSMD_LB_IMGUI, "free imgui context.");
    }

    void RenderImGui::RenderGuiContextA() {
        // start the imgui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void RenderImGui::RenderGuiContextB() {
        // render imgui.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}