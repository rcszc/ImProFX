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
> By RCSZ 2024 - 2025.
)";

using namespace ImGuiPro;
namespace ImProFXgui {

#define DEFPARAM_TP_LSA_SPEED     0.032f
#define DEFPARAM_TP_LSA_POINTSIZE 10.0f
    void FrameworkGUI::PRORZ_TemplatePageLSA(
        const char*             name,
        ImVec2                  winsize,
        const vector<uint32_t>& pageimgs,
        const Vector2T<bool>&   playflag,
        Vector2T<float>&        xpos,
        bool                    border,
        float                   transparent,
        ImGuiWindowFlags        winflags
    ) {
        float PlayStepLength = winsize.x + IMGUI_ITEM_SPC;

        ImGuiWindowChild(name,
            [&]() {
                for (const auto& item : pageimgs) {
                    // draw full window images.
                    Image(IMTOOL_CVT_IMPTR(item), winsize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, transparent));
                    SameLine();
                }

                if (playflag.vector_x) {
                    xpos.vector_x -= PlayStepLength;
                    xpos.vector_x = xpos.vector_x < 0.0f ? 0.0f : xpos.vector_x;
                }

                if (playflag.vector_y) {
                    xpos.vector_x += PlayStepLength;
                    xpos.vector_x = xpos.vector_x > (PlayStepLength * float(pageimgs.size() - 1)) ?
                        (PlayStepLength * float(pageimgs.size() - 1)) : xpos.vector_x;
                }

                // positioning.
                if (abs(xpos.vector_x - xpos.vector_y) < DEFPARAM_TP_LSA_SPEED * 2.0f)
                    xpos.vector_y = xpos.vector_x;

                xpos.vector_y += (xpos.vector_x - xpos.vector_y) * DEFPARAM_TP_LSA_SPEED;
                ImGui::SetScrollX(xpos.vector_y);

                ImVec2 PosTemp = ImVec2(
                    PRO_ItemCalcCentered(float(pageimgs.size()) * (DEFPARAM_TP_LSA_POINTSIZE + IMGUI_ITEM_SPC * 0.5f)) + xpos.vector_y,
                    winsize.y - 2.0f
                );
                for (size_t i = 0; i < pageimgs.size(); ++i) {
                    SetCursorPos(ImVec2(PosTemp.x + (float)i * IMGUI_ITEM_SPC * 1.5f, PosTemp.y));

                    if (size_t(xpos.vector_x / PlayStepLength) == i)
                        PRO_LndicatorLED(true, ImVec2(DEFPARAM_TP_LSA_POINTSIZE, DEFPARAM_TP_LSA_POINTSIZE));
                    else
                        PRO_LndicatorLED(false, ImVec2(DEFPARAM_TP_LSA_POINTSIZE, DEFPARAM_TP_LSA_POINTSIZE));
                }
            },
            ImVec2(winsize.x + IMGUI_ITEM_SPC, winsize.y + IMGUI_ITEM_SPC * 2.0f), border, winflags | ImGuiWindowFlags_NoScrollWithMouse
        );
    }

#define FDSA_CHILD_WIN_HIGH 36.0f
    void FrameworkGUI::PRORZ_TemplatePageSortA(
        const char*                         name,
        const unordered_map<string, float>& data,
        bool                                border,
        const ImVec4&                       color
    ) {
        // 哈希表转为键值对 vector => sort.
        vector<pair<string, float>> DataTemp(data.begin(), data.end());
        sort(DataTemp.begin(), DataTemp.end(), [](const auto& pair1, const auto& pair2) { return pair1.second < pair2.second; });

        float WindowSizeHeight = (float)DataTemp.size() * (FDSA_CHILD_WIN_HIGH + IMGUI_ITEM_SPC) + IMGUI_ITEM_SPC;

        ImGuiWindowChild(name,
            [&]() {
                float ValueMin = DataTemp.front().second;
                float ValueMax = DataTemp.back().second;

                int ItemCount = NULL;
                for (int i = (int)DataTemp.size() - 1; i > -1; --i) {

                    float value_range = ValueMax - ValueMin;
                    float value_offset = abs(ValueMin);
                    float value_scale = (DataTemp[i].second + value_offset) / (value_range + 2.0f * value_offset);

                    PushStyleColor(ImGuiCol_PlotHistogram, color);
                    {
                        SetCursorPosY((float)ItemCount * (FDSA_CHILD_WIN_HIGH + IMGUI_ITEM_SPC) + IMGUI_ITEM_SPC);
                        ++ItemCount;
                        ImGuiWindowChild(string(to_string(i)).c_str(),
                            [&]() {
                                // 文字靠向进度条.
                                ImGui::Indent(100.0f - ImGui::CalcTextSize(DataTemp[i].first.c_str()).x - IMGUI_ITEM_SPC * 2.0f);
                                ImGui::Text(DataTemp[i].first.c_str());
                            },
                            ImVec2(100.0f, FDSA_CHILD_WIN_HIGH), border
                        );

                        SameLine();
                        ProgressBar(value_scale, ImVec2(384.0f, 32.0f), "");
                        SameLine();
                        Text("%.2f", DataTemp[i].second);
                    }
                    PopStyleColor();
                }
            },
            ImVec2(640.0f, WindowSizeHeight), border
        );
    }

    void FrameworkGUI::PRORZ_TemplatePageLayout169A(
        const char*      name,
        function<void()> child_win_0,
        function<void()> child_win_1,
        function<void()> child_win_2,
        function<void()> child_win_3,
        float            global_scale,
        bool             border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE169.vector_x * global_scale, WIN_PRESET_SIZE169.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC; // 子窗口间距.

        ImGuiWindowChild(name,
            [&]() {
                SetCursorPos(ImVec2(WindowSPC, WindowSPC));
                ImGuiWindowChild("NULLSTR", child_win_0, ImVec2(WindowSize.x * 0.2f, WindowSize.y - WindowSPC * 2.0f), border);

                SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.2f, WindowSPC));
                ImGuiWindowChild("NULLSTR", child_win_1, ImVec2(WindowSize.x * 0.8f - WindowSPC * 3.0f, WindowSize.y * 0.65f), border);

                SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.2f, WindowSize.y * 0.65f + WindowSPC * 2.0f));
                ImGuiWindowChild("NULLSTR", child_win_2, ImVec2(WindowSize.x * 0.6f - WindowSPC * 3.0f, WindowSize.y * 0.35f - WindowSPC * 3.0f), border);

                SetCursorPos(ImVec2(WindowSize.x * 0.8f, WindowSize.y * 0.65f + WindowSPC * 2.0f));
                ImGuiWindowChild("NULLSTR", child_win_3, ImVec2(WindowSize.x * 0.2f - WindowSPC, WindowSize.y * 0.35f - WindowSPC * 3.0f), border);
            },
            WindowSize, border
        );
    }

    void FrameworkGUI::PRORZ_TemplatePageLayout169B(
        const char*      name,
        function<void()> child_win_0,
        function<void()> child_win_1,
        function<void()> child_win_2,
        function<void()> child_win_3,
        float            global_scale,
        bool             border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE169.vector_x * global_scale, WIN_PRESET_SIZE169.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC;

        ImGuiWindowChild(name,
            [&]() {
                SetCursorPos(ImVec2(WindowSPC, WindowSPC));
                ImGuiWindowChild("NULLSTR", child_win_0, ImVec2(WindowSize.x - WindowSPC * 2.0f, WindowSize.y * 0.15f - WindowSPC), border);

                SetCursorPos(ImVec2(WindowSPC, WindowSPC + WindowSize.y * 0.15f));
                ImGuiWindowChild("NULLSTR", child_win_1, ImVec2(WindowSize.x * 0.25f - WindowSPC, WindowSize.y * 0.85f - WindowSPC * 2.0f), border);

                SetCursorPos(ImVec2(WindowSPC + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.15f));
                ImGuiWindowChild("NULLSTR", child_win_2, ImVec2(WindowSize.x * 0.75f - WindowSPC * 2.0f, WindowSize.y * 0.6f - WindowSPC), border);

                SetCursorPos(ImVec2(WindowSPC + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.75f));
                ImGuiWindowChild("NULLSTR", child_win_3, ImVec2(WindowSize.x * 0.75f - WindowSPC * 2.0f, WindowSize.y * 0.25f - WindowSPC * 2.0f), border);
            },
            WindowSize, border
        );
    }

    void FrameworkGUI::PRORZ_TemplatePageLayout1216A(
        const char*           name,
        std::function<void()> child_win_0,
        std::function<void()> child_win_1,
        std::function<void()> child_win_2,
        float                 global_scale,
        bool                  border
    ) {
        ImVec2 WindowSize = ImVec2(WIN_PRESET_SIZE1216.vector_x * global_scale, WIN_PRESET_SIZE1216.vector_y * global_scale);
        const float WindowSPC = IMGUI_ITEM_SPC;

        ImGuiWindowChild(name,
            [&]() {
                ImGui::SetCursorPos(ImVec2(WindowSPC, WindowSPC));
                ImGuiWindowChild("NULLSTR", child_win_0, ImVec2(WindowSize.x * 0.25f, WindowSize.y - WindowSPC * 2.0f), border);

                ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.25f, WindowSPC));
                ImGuiWindowChild("NULLSTR", child_win_1, ImVec2(WindowSize.x * 0.75f - WindowSPC * 3.0f, WindowSize.y * 0.85f - WindowSPC), border);

                ImGui::SetCursorPos(ImVec2(WindowSPC * 2.0f + WindowSize.x * 0.25f, WindowSPC + WindowSize.y * 0.85f));
                ImGuiWindowChild("NULLSTR", child_win_2, ImVec2(WindowSize.x * 0.75f - WindowSPC * 3.0f, WindowSize.y * 0.15f - WindowSPC * 2.0f), border);
            }, 
            WindowSize, border
        );
    }

#define DEBUG_ITEM_WIDTH 208.0f
    void FrameworkGUI::RZsystemDebugWindow(DebugWindowBuffer& buffer, bool* p_open) {

        PushStyleColor(ImGuiCol_WindowBg,         ImVec4(0.1f, 0.0f, 0.24f, 0.92f));
        PushStyleColor(ImGuiCol_TitleBg,          ImVec4(0.58f, 0.0f, 1.0f, 0.42f));
        PushStyleColor(ImGuiCol_TitleBgActive,    ImVec4(0.58f, 0.0f, 1.0f, 0.42f));
        PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.58f, 0.0f, 1.0f, 0.42f));

        SetNextWindowSize(ImVec2(350.0f, 400.0f));
        ImGuiWindow("> Framework22 ImGui Debug",
            [&]() {
                Text("ImGui RenderFps: %0.3f", GetImGuiIO().Framerate);
                Text("ImGui MousePos: [ %d,%d ]", (int)GetImGuiIO().MousePos.x, (int)GetImGuiIO().MousePos.y);
                Text("ImGui Frame: %d", ImGui::GetFrameCount());

                buffer.FpsSampleBuffer[0] = ImGui::GetIO().Framerate;
                for (size_t i = DEBUG_FPS_SAMPLE_LEN - 1; i > 0; i--)
                    buffer.FpsSampleBuffer[i] = buffer.FpsSampleBuffer[i - 1];

                SetNextItemWidth(DEBUG_ITEM_WIDTH);
                PlotLines("Fps.Sample", buffer.FpsSampleBuffer, DEBUG_FPS_SAMPLE_LEN, NULL, (const char*)0, 0.0f, 320.0f, ImVec2(0.0f, 42.0f));

                SetNextItemWidth(DEBUG_ITEM_WIDTH);
                InputInt(InputInt1, "View.Log.Lines", &buffer.LogWindowSmpLines);
                // lines limit: 0 < x < n.
                buffer.LogWindowSmpLines = buffer.LogWindowSmpLines < 2 ? 2 : buffer.LogWindowSmpLines;

                if (Button("View Log", ImVec2(128.0f, 42.0f)))
                    buffer.LogWindowOpenFlag = !buffer.LogWindowOpenFlag;

                // state display.
                SameLine(); PRO_LndicatorLED(buffer.LogWindowOpenFlag,             ImVec2(16.0f, 42.0f));
                SameLine(); PRO_LndicatorLED(IsMouseDown(ImGuiMouseButton_Left),   ImVec2(42.0f, 42.0f));
                SameLine(); PRO_LndicatorLED(IsMouseDown(ImGuiMouseButton_Middle), ImVec2(42.0f, 42.0f));
                SameLine(); PRO_LndicatorLED(IsMouseDown(ImGuiMouseButton_Right),  ImVec2(42.0f, 42.0f));

                if (buffer.LogWindowOpenFlag) {
                    SetNextWindowSize(ImVec2(820.0f, 768.0f));
                   
                    ImGuiWindow(string("> ImProFx Log " + to_string(buffer.LogWindowSmpLines) + " Lines").c_str(),
                        [&]() {
                            ImGui::SetWindowFontScale(0.872f);
                            ImVec4 color = ImVec4();
                            for (const auto& logstr : LOGCONS::ReadLogCache::ExtractLogSegment(buffer.LogWindowSmpLines)) {

                                switch (logstr.LogLabel) {
                                case(LOG_INFO):    { color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f); break; };
                                case(LOG_ERROR):   { color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f); break; };
                                case(LOG_WARNING): { color = ImVec4(1.0f, 1.0f, 0.0f, 0.72f); break; };
                                case(LOG_TRACE):   { color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f); break; };
                                case(LOG_PERF):    { color = ImVec4(0.78f, 0.0f, 1.0f, 0.72f); break; };
                                }

                                TextColored(color, logstr.LogString.c_str());
                            }
                        },
                        &buffer.LogWindowOpenFlag, ImGuiWindowFlags_NoDecoration
                    );
                }
                Text(SystemDefaultImProFx);
            },
            p_open, ImGuiWindowFlags_NoResize
        );
        PopStyleColor(4);
    }
}