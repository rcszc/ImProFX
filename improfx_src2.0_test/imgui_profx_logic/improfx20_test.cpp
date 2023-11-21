// improfx20_test.
#include "improfx20_test.h"

using namespace std;

using namespace ProFxFramework;

string teststr = u8R"(
 ImGuiProFX - II, Version 2.0.0 TEST @RCSZ 2023 - 2024.
 Libray: OpenGL GLFW, OpenGL GLEW, ImGui, RapidJSON, stb_image.
 Msvc C++17 Release x64, IDE: Visual Studio 2022.
--------------------------------------------------------
 基于ImGui和OpenGL着色器的GUI框架, 为提升原版ImGui的视角效果以及体验.
 为开发者提供更多工具, 保留ImGui的即时性和简洁.
 相较于1.x版本新增了: 异步任务, 线程池, 反射, 内联着色器, 扩展控件.
 相较于1.x版本完善了: 日志系统, 内存对象, 着色器组件.

 GitHub(BSD3): https://github.com/rcszc/ImProFX
 所有shader实例都来自: https://www.shadertoy.com/
 2.0.0 TEST 相较于 Ahpla 还是确实很多功能, 属于前瞻版本, 争取在过年前肝完.
 - 2023.11.21
)";

bool TestUserClass::LogicInitialization(FRMCORE_PACKAGE& FrmDat) {
	
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_main"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_ref"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_thp"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_ils"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_shd"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_log"));
	ImProFX20page.push_back(FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "20page_end"));
	
	auto page_button_front = ImGuiProAnim::CreateAnimButton();
	
	page_button_front->config_normal_size = Vector2T<float>(142.0f, 58.0f);
	page_button_front->config_hover_size  = Vector2T<float>(150.0f, 64.0f);
	page_button_front->config_active_size = Vector2T<float>(128.0f, 42.0f);

	page_button_front->config_normal_color = Vector4T<float>(1.0f, 0.16f, 0.0f, 0.72f);
	page_button_front->config_hover_color  = Vector4T<float>(0.48f, 0.0f, 1.0f, 0.85f);
	page_button_front->config_active_color = Vector4T<float>(0.0f, 1.0f, 0.72f, 1.0f);

	auto page_button_back = ImGuiProAnim::CreateAnimButton(page_button_front);

	auto debug_button = ImGuiProAnim::CreateAnimButton(page_button_front);

	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "pbut_front", page_button_front);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "pbut_back", page_button_back);
	ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "but_debug", debug_button);

	FrmStatic.StaticFlagAlloc(FrmDat.StaticFlag, "debug_flag");
	
	return true;
}

bool TestUserClass::LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) {
	
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.32f, 0.0f, 0.52f));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	// 无标题栏, 固定位置, 固定大小, 无法折叠, 隐藏滑条, 不接受鼠标滚轮事件.
	ImGuiWindowFlags TmpFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("2.0 TEST", nullptr, TmpFlags);
	ImGuiPro::FullWindowBackground(FrmData.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_sunsky_shader"), ImVec2(0.85f, 1.0f), 8.0f);
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGuiPro::SmoothYSlide(window_page_ypos);

		// 标题居中.
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(950.0f));
		ImGuiPro::TextTitleP1("IMGUI PRO FX - II Framework Version 2.0.0 TEST @RCSZ", 950.0f, ImVec4(0.0f, 1.0f, 0.72f, 0.88f));

		// 主页面居中.
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(1440.0f));
		ImGuiProRZ::TemplatePageLSA("TestLSA", ImVec2(1440.0f, 810.0f), ImProFX20page, demo_page_play, demo_page_xpos, false, 0.7f);

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(300.0f));
		// 页面滚动按钮.
		demo_page_play.vector_x = ImGuiProAnim::CallAnimButton("Front", FrmDat.DataAnimation, "pbut_front");
		ImGui::SameLine();
		demo_page_play.vector_y = ImGuiProAnim::CallAnimButton("Back", FrmDat.DataAnimation, "pbut_back");

		ImGui::Text("");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(360.0f));
		ImGuiPro::TextTitleP2("Framework Debug Window", 360.0f, ImVec4(0.0f, 1.0f, 0.72f, 0.88f));

		auto debug_flag = FrmStatic.StaticFlagFind(FrmDat.StaticFlag, "debug_flag");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(150.0f));
		if (ImGuiProAnim::CallAnimButton("Debug", FrmDat.DataAnimation, "but_debug"))
			*debug_flag = !*debug_flag;

		if (*debug_flag)
			ImGuiProRZ::RZsystemDebugWindow(debug_flag);

		ImGui::Text("");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(385.0f));
		ImGuiPro::TextTitleP2("Framework Template Window", 385.0f, ImVec4(0.0f, 1.0f, 0.72f, 0.88f));

		if (ImGuiPro::DetePosScrollY(ImVec2(900.0f, 5056.0f)))
			slider_bg += (0.12f - slider_bg) * 0.05f;
		else
			slider_bg += (1.0f - slider_bg) * 0.05f;
		FrmData.FindSetRenderItemParam(FrmDat.DataShaderFx, "fx_sunsky_shader", "lightning", slider_bg);

		ImGui::Text("");
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(256.0f));
		ImGui::SetNextItemWidth(256.0f);
		ImGuiPro::InterSliderFloat("Sacle[TPA169A]", slider_value[0], ImVec2(0.5f, 2.0f));

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(ImGuiProRZ::WIN_PRESET_SIZE169.vector_x * slider_value[0].vector_y));
		ImGuiProRZ::TemplatePageApp169A(
			"TPA169A-Test",
			[]() {},
			[]() {},
			[]() {},
			[]() {},
			slider_value[0].vector_y
		);

		ImGui::Text("");
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(256.0f));
		ImGui::SetNextItemWidth(256.0f);
		ImGuiPro::InterSliderFloat("Sacle[TPA169B]", slider_value[1], ImVec2(0.5f, 2.0f));
		
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(ImGuiProRZ::WIN_PRESET_SIZE169.vector_x * slider_value[1].vector_y));
		ImGuiProRZ::TemplatePageApp169B(
			"TPA169B-Test",
			[]() {},
			[]() {},
			[]() {},
			[]() {},
			slider_value[1].vector_y
		);

		ImGui::Text("");
		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(256.0f));
		ImGui::SetNextItemWidth(256.0f);
		ImGuiPro::InterSliderFloat("Sacle[TPA1216A]", slider_value[2], ImVec2(0.5f, 2.0f));

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(ImGuiProRZ::WIN_PRESET_SIZE1216.vector_x * slider_value[2].vector_y));
		ImGuiProRZ::TemplatePageApp1216A(
			"TPA1216A-Test",
			[]() {},
			[]() {},
			[]() {},
			slider_value[2].vector_y
		);

		ImGui::Text("");

		static unordered_map<string, float> TestFDSA = {
			{"TestA", 7983.0f},
			{"TestB", 2476.0f},
			{"TestC", 9981.0f},
			{"TestD", 1032.0f},
			{"TestE", 786.0f},
			{"TestF", 15237.0f}
		};

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(640.0f));
		ImGuiProRZ::TemplatePageFDSA("TestFDSA", TestFDSA, true);

		ImGui::Text("");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(385.0f));
		ImGuiPro::TextTitleP2("Framework Text Animation", 385.0f, ImVec4(0.0f, 1.0f, 0.72f, 0.88f));

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(768.0f));
		ImGuiPro::ViewTextMultilineAnim(
			"##TestTM", 
			teststr, 
			multiline_anim_count, 
			ImGuiPro::DetePosScrollY(ImVec2(2550.0f, 3815.0f)), 
			ImVec2(768.0f, 272.0f),
			2.0f
		);

		ImGui::Text("");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(340.0f));
		ImGuiPro::TextTitleP2("Framework Shader Demo", 340.0f, ImVec4(0.0f, 1.0f, 0.72f, 0.88f));

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(1040.0f));
		ImGui::BeginChild("shaderdemo1", ImVec2(1040.0f, 592.0f));
		{
			ImGuiPro::RoundImage(
				FrmData.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_circle_shader"),
				ImGui::GetCursorPos(),
				ImVec2(1024.0f, 576.0f)
			);
		}
		ImGui::EndChild();

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(1040.0f));
		ImGui::BeginChild("shaderdemo2", ImVec2(1040.0f, 592.0f));
		{
			ImGuiPro::RoundImage(
				FrmData.FindRenderItemFBO(FrmDat.DataShaderFx, "fx_background_shader"),
				ImGui::GetCursorPos(),
				ImVec2(1024.0f, 576.0f)
			);
		}
		ImGui::EndChild();

		ImGui::Text("");

		ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(384.0f));
		ImGuiPro::TextTitleP2(u8"ImProFX - II 具体请参考文档.", 384.0f, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));

		ImGui::BeginChild("NULL END", ImVec2(1550.0f, 900.0f));
		// 占位子窗口.
		ImGui::EndChild();
		
		ImGuiPro::EndEnterLine();
	}
	ImGui::End();

	return true;
}

void TestUserClass::LogicCloseFree() {
	
}
