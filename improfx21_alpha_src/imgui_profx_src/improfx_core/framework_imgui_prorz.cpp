// framework_imgui_prorz.
#include <algorithm> // 快排

#include "framework_imgui.h"

using namespace std;
using namespace LOGCONS;

const char* SystemDefaultImProFx = u8R"(
> Debug LED RectangleState: OpenLog
> Debug LED SquareState:
    - mouse button left
    - mouse button middle
    - mouse button right 
> C++17 MSVC x64
> OpenGL 4.6, GLFW 3.3
> By RCSZ 2023 - 2024.
)";

using namespace ImGuiPro;
namespace ImGuiProRZ {

#define DEFPARAM_TP_LSA_SPEED     0.028f
#define DEFPARAM_TP_LSA_POINTSIZE 10.0f
    void TemplatePageLSA(
        const char*             name,
        ImVec2                  winsize,
        const vector<uint32_t>& pageimgs,
        const Vector2T<bool>&   playflag,
        Vector2T<float>&        xpos,
        const bool&             border,
        const float&            transparent,
        ImGuiWindowFlags        winflags
    ) {
        float palystep = winsize.x + IMGUI_ITEM_SPC;

        ImGui::BeginChild(name, ImVec2(winsize.x + IMGUI_ITEM_SPC, winsize.y + IMGUI_ITEM_SPC * 2.0f), border, winflags);
        {
            for (const auto& item : pageimgs) {
                // draw full window images.
                ImGui::Image(CVT_IMPTR(item), winsize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, transparent));
                ImGui::SameLine();
            }

            if (playflag.vector_x) {
                xpos.vector_x -= palystep;
                xpos.vector_x = xpos.vector_x < 0.0f ? 0.0f : xpos.vector_x;
            }

            if (playflag.vector_y) {
                xpos.vector_x += palystep;
                xpos.vector_x = xpos.vector_x > (palystep * float(pageimgs.size() - 1)) ? 
                    (palystep * float(pageimgs.size() - 1)) : xpos.vector_x;
            }

            // positioning.
            if (abs(xpos.vector_x - xpos.vector_y) < DEFPARAM_TP_LSA_SPEED * 2.0f)
                xpos.vector_y = xpos.vector_x;

            xpos.vector_y += (xpos.vector_x - xpos.vector_y) * DEFPARAM_TP_LSA_SPEED;
            ImGui::SetScrollX(xpos.vector_y);

            ImVec2 tmprepos = ImVec2(
                ItemCalcCentered(float(pageimgs.size()) * (DEFPARAM_TP_LSA_POINTSIZE + IMGUI_ITEM_SPC * 0.5f)) + xpos.vector_y, 
                winsize.y - 2.0f
            );
            for (size_t i = 0; i < pageimgs.size(); ++i) {
                ImGui::SetCursorPos(ImVec2(tmprepos.x + (float)i * IMGUI_ITEM_SPC * 1.5f, tmprepos.y));

                if (size_t(xpos.vector_x / palystep) == i) 
                    LndicatorLED(true,  ImVec2(DEFPARAM_TP_LSA_POINTSIZE, DEFPARAM_TP_LSA_POINTSIZE));
                else                                       
                    LndicatorLED(false, ImVec2(DEFPARAM_TP_LSA_POINTSIZE, DEFPARAM_TP_LSA_POINTSIZE));
            }
        }
        ImGui::EndChild();
    }

#define FDSA_CHILD_WIN_HIGH 36.0f
    void TemplatePageFDSA(
        const char*                         name,
        const unordered_map<string, float>& data,
        const bool&                         border,
        const ImVec4&                       color
    ) {
        // 哈希表转为键值对vector => 排序.
        vector<pair<string, float>> temp_data(data.begin(), data.end());
        sort(temp_data.begin(), temp_data.end(), [](const auto& pair1, const auto& pair2) { return pair1.second < pair2.second; });

        float WindowSizeHeight = (float)temp_data.size() * (FDSA_CHILD_WIN_HIGH + IMGUI_ITEM_SPC) + IMGUI_ITEM_SPC;

        ImGui::BeginChild(name, ImVec2(640.0f, WindowSizeHeight), border);
        {
            float value_min = temp_data.front().second;
            float value_max = temp_data.back().second;
            
            int count = NULL;
            for (int i = (int)temp_data.size() - 1; i > -1; --i) {
                
                float value_range  = value_max - value_min;
                float value_offset = abs(value_min);
                float value_scale  = (temp_data[i].second + value_offset) / (value_range + 2.0f * value_offset);

                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
                {
                    ImGui::SetCursorPosY((float)count * (FDSA_CHILD_WIN_HIGH + IMGUI_ITEM_SPC) + IMGUI_ITEM_SPC);
                    ++count;
                    ImGui::BeginChild(string(to_string(i)).c_str(), ImVec2(100.0f, FDSA_CHILD_WIN_HIGH), border);
                    {
                        // 文字靠向进度条.
                        ImGui::Indent(100.0f - ImGui::CalcTextSize(temp_data[i].first.c_str()).x - IMGUI_ITEM_SPC * 2.0f);
                        ImGui::Text(temp_data[i].first.c_str());
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::ProgressBar(value_scale, ImVec2(384.0f, 32.0f), "");
                    ImGui::SameLine();
                    ImGui::Text("%.2f", temp_data[i].second);
                }
                ImGui::PopStyleColor();
            }
        }
        ImGui::EndChild();
    }

    void TemplatePageApp169A(
        const char*      unique_name,
        function<void()> child_win_0,
        function<void()> child_win_1,
        function<void()> child_win_2,
        function<void()> child_win_3,
        const float&     global_scale,
        const bool&      border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE169.vector_x * global_scale, WIN_PRESET_SIZE169.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC; // 子窗口间距.

        ImGui::BeginChild(unique_name, WindowSize, border);
        {
            ImGui::SetCursorPos(ImVec2(WindowSPC, WindowSPC));
            ImGui::BeginChild("##TPAAw0", ImVec2(WindowSize.x * 0.2f, WindowSize.y - WindowSPC * 2.0f), border);
            child_win_0();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.2f, WindowSPC));
            ImGui::BeginChild("##TPAAw1", ImVec2(WindowSize.x * 0.8f - WindowSPC * 3.0f, WindowSize.y * 0.65f), border);
            child_win_1();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.2f, WindowSize.y * 0.65f + WindowSPC * 2.0f));
            ImGui::BeginChild("##TPAAw2", ImVec2(WindowSize.x * 0.6f - WindowSPC * 3.0f, WindowSize.y * 0.35f - WindowSPC * 3.0f), border);
            child_win_2();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSize.x * 0.8f, WindowSize.y * 0.65f + WindowSPC * 2.0f));
            ImGui::BeginChild("##TPAAw3", ImVec2(WindowSize.x * 0.2f - WindowSPC, WindowSize.y * 0.35f - WindowSPC * 3.0f), border);
            child_win_3();
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

    void TemplatePageApp169B(
        const char*      unique_name,
        function<void()> child_win_0,
        function<void()> child_win_1,
        function<void()> child_win_2,
        function<void()> child_win_3,
        const float&     global_scale,
        const bool&      border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE169.vector_x * global_scale, WIN_PRESET_SIZE169.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC;

        ImGui::BeginChild(unique_name, WindowSize, border);
        {
            ImGui::SetCursorPos(ImVec2(WindowSPC, WindowSPC));
            ImGui::BeginChild("##TPABw0", ImVec2(WindowSize.x - WindowSPC * 2.0f, WindowSize.y * 0.15f - WindowSPC), border);
            child_win_0();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC, WindowSPC + WindowSize.y * 0.15f));
            ImGui::BeginChild("##TPABw1", ImVec2(WindowSize.x * 0.25f - WindowSPC, WindowSize.y * 0.85f - WindowSPC * 2.0f), border);
            child_win_1();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.15f));
            ImGui::BeginChild("##TPABw2", ImVec2(WindowSize.x * 0.75f - WindowSPC * 2.0f, WindowSize.y * 0.6f - WindowSPC), border);
            child_win_2();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.75f));
            ImGui::BeginChild("##TPABw3", ImVec2(WindowSize.x * 0.75f - WindowSPC * 2.0f, WindowSize.y * 0.25f - WindowSPC * 2.0f), border);
            child_win_3();
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

    void TemplatePageApp1216A(
        const char*           unique_name,
        std::function<void()> child_win_0,
        std::function<void()> child_win_1,
        std::function<void()> child_win_2,
        const float&          global_scale,
        const bool&           border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE1216.vector_x * global_scale, WIN_PRESET_SIZE1216.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC;

        ImGui::BeginChild(unique_name, WindowSize, border);
        {
            ImGui::SetCursorPos(ImVec2(WindowSPC, WindowSPC));
            ImGui::BeginChild("##TPAAw0s", ImVec2(WindowSize.x * 0.25f, WindowSize.y - WindowSPC * 2.0f), border);
            child_win_0();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.25f, WindowSPC));
            ImGui::BeginChild("##TPAAw1s", ImVec2(WindowSize.x * 0.75f - WindowSPC * 3.0f, WindowSize.y * 0.85f - WindowSPC), border);
            child_win_1();
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.85f));
            ImGui::BeginChild("##TPAAw2s", ImVec2(WindowSize.x * 0.75f - WindowSPC * 3.0f, WindowSize.y * 0.15f - WindowSPC * 2.0f), border);
            child_win_2();
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

#define FPS_SAMPLE_LEN   128
#define DEBUG_ITEM_WIDTH 208.0f
    void RZsystemDebugWindow(bool* p_open) {

        static bool   logwindow_flag                   = false;
        static int    logwindow_lines                  = 72;
        static float  fpssample_buffer[FPS_SAMPLE_LEN] = {};

        ImGui::PushStyleColor(ImGuiCol_WindowBg,         ImVec4(0.1f, 0.0f, 0.24f, 0.92f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg,          ImVec4(0.58f, 0.0f, 1.0f, 0.42f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive,    ImVec4(0.58f, 0.0f, 1.0f, 0.42f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.58f, 0.0f, 1.0f, 0.42f));

        ImGui::SetNextWindowSize(ImVec2(350.0f, 400.0f));
        ImGui::Begin("> Framework20 ImGui Debug", p_open, ImGuiWindowFlags_NoResize);
        {
            ImGui::Text("ImGui RenderFps: %0.3f", ImGui::GetIO().Framerate);
            ImGui::Text("ImGui MousePos: [ %d,%d ]", (int)ImGui::GetIO().MousePos.x, (int)ImGui::GetIO().MousePos.y);
            ImGui::Text("ImGui Frame: %d", ImGui::GetFrameCount());

            fpssample_buffer[0] = ImGui::GetIO().Framerate;
            for (size_t i = FPS_SAMPLE_LEN - 1; i > 0; i--)
                fpssample_buffer[i] = fpssample_buffer[i - 1];

            ImGui::SetNextItemWidth(DEBUG_ITEM_WIDTH);
            ImGui::PlotLines("Fps.Sample", fpssample_buffer, FPS_SAMPLE_LEN, NULL, (const char*)0, 0.0f, 320.0f, ImVec2(0.0f, 42.0f));

            ImGui::SetNextItemWidth(DEBUG_ITEM_WIDTH);
            ImGui::InputInt("View.Log.Lines", &logwindow_lines);
            // lines limit: 0 < x < n.
            logwindow_lines = logwindow_lines < 2 ? 2 : logwindow_lines;

            if (ImGui::Button("View Log", ImVec2(128.0f, 42.0f)))
                logwindow_flag = !logwindow_flag;

            // state display.
            ImGui::SameLine(); ImGuiPro::LndicatorLED(logwindow_flag,                              ImVec2(16.0f, 42.0f));
            ImGui::SameLine(); ImGuiPro::LndicatorLED(ImGui::IsMouseDown(ImGuiMouseButton_Left),   ImVec2(42.0f, 42.0f));
            ImGui::SameLine(); ImGuiPro::LndicatorLED(ImGui::IsMouseDown(ImGuiMouseButton_Middle), ImVec2(42.0f, 42.0f));
            ImGui::SameLine(); ImGuiPro::LndicatorLED(ImGui::IsMouseDown(ImGuiMouseButton_Right),  ImVec2(42.0f, 42.0f));

            if (logwindow_flag) {
                ImGui::SetNextWindowSize(ImVec2(820.0f, 768.0f));
                ImGui::Begin(
                    string("> ImProFx Log " + to_string(logwindow_lines) + " Lines").c_str(), 
                    &logwindow_flag,
                    ImGuiWindowFlags_NoDecoration
                );

                ImGui::SetWindowFontScale(0.872f);
                ImVec4 color = ImVec4();
                for (const auto& logstr : LOGCONS::ReadLogCache::ExtractLogSegment(logwindow_lines)) {

                    switch (logstr.LogLabel) {
                    case(LOG_INFO): { color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f); break; };
                    case(LOG_ERR):  { color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f); break; };
                    case(LOG_WARN): { color = ImVec4(1.0f, 1.0f, 0.0f, 0.72f); break; };
                    case(LOG_TRC):  { color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f); break; };
                    case(LOG_PERF): { color = ImVec4(0.78f, 0.0f, 1.0f, 0.72f); break; };
                    }

                    ImGui::TextColored(color, logstr.LogString.c_str());
                }
                ImGui::End();
            }
            ImGui::Text(SystemDefaultImProFx);
        }
        ImGui::End();

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}