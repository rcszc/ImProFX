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

        if (!std::filesystem::exists(cfgdata.FontsFilepath))
            PushLogger(LOG_WARNING, SYSMD_LB_IMGUI, "failed read fonts file.");
        else
            PushLogger(LOG_INFO, SYSMD_LB_IMGUI, "read fonts file: ", cfgdata.FontsFilepath);

        // setup imgui style.
        ImGui::StyleColorsDark();
        {
            ImGuiStyle* ConfigStyle = &ImGui::GetStyle();
            ImVec4* ConfigTextColor = ConfigStyle->Colors;

            ConfigTextColor[ImGuiCol_Text] = ImVec4(
                cfgdata.FontsGlobalColor.vector_x,
                cfgdata.FontsGlobalColor.vector_y,
                cfgdata.FontsGlobalColor.vector_z,
                cfgdata.FontsGlobalColor.vector_w
            );
        }
        // init set font.
        auto ConfigFonts = ImGui::GetIO().Fonts;
        ConfigFonts->AddFontFromFileTTF(
            cfgdata.FontsFilepath.c_str(),
            cfgdata.FontsGlobalSize,
            NULL,
            ConfigFonts->GetGlyphRangesChineseFull()
        );
        ImGui::GetIO().FontGlobalScale = 0.68f;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, cfgdata.WindowRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfgdata.FrameRounding);

        // setup platform / renderer backends.
        ImGui_ImplGlfw_InitForOpenGL(window_object, true);
        ImGui_ImplOpenGL3_Init(cfgdata.ShaderVersionStr.c_str());
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